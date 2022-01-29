import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, jsonify
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd, appreciation

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd
app.jinja_env.filters["appreciation"] = appreciation

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")
db.execute("CREATE TABLE IF NOT EXISTS stocks (id INTEGER PRIMARY KEY, session_id INTEGER, symbol TEXT NOT NULL, shares INTEGER NOT NULL, price NUMERIC NOT NULL, total NUMERIC NOT NULL DEFAULT 0, bought_total NUMERIC NOT NULL DEFAULT 0, sold NUMERIC NOT NULL DEFAULT 0, operation TEXT NOT NULL, FOREIGN KEY(session_id) REFERENCES users(id))")
db.execute("CREATE TABLE IF NOT EXISTS invest (id INTEGER PRIMARY KEY, session_id INTEGER, invested NUMERIC NOT NULL DEFAULT 10000, FOREIGN KEY(session_id) REFERENCES users(id))")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/", methods=["GET", "POST"])
@login_required
def index():
    """Show portfolio of stocks"""
    stocks = db.execute(
        "SELECT SUM(shares) as shares, symbol, SUM(bought_total) as bought_total, SUM(total) as total FROM stocks WHERE session_id = ? GROUP BY symbol HAVING (SUM(shares)) > 0", session["user_id"])
    cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])

    # try to access the invest table for the first time, if not possible, create the first row
    try:
        invested = db.execute("SELECT invested FROM invest WHERE session_id = ? ", session["user_id"])
        invested_cash = invested[0]["invested"]
    except IndexError:
        db.execute("INSERT INTO invest (session_id) VALUES (?)", session["user_id"])
        invested = db.execute("SELECT invested FROM invest WHERE session_id = ? ", session["user_id"])
        invested_cash = invested[0]["invested"]

    """
    https://www.investopedia.com/articles/investing/060313/what-determines-your-cost-basis.asp
    The formula for the amount earned is -> amountOfShares*sharesSellPrice - (cost_basis).
    Cost basis is the  (amountSharesBought * boughtPrice), if a user bought more than one time with different bought price, the cost basis is
    (amountOfShares1 * boughtPrice1) + (amountOfShares2 * boughtPrice2). So you do a FIFO sell with the bought Shares and then you get your earned profit.
    e.g.: bought 10 shares for 1$ from NFLX, then bought 5 shares for 5$ from NFLX, want to sell 12 shares for 2$ -> totalProfit = 12 * 2$ - (10 * 1$ + 2 * 5$) = 4$ profit """
    cost_basis = 0

    current_stock_cash = 0

    for stock in stocks:
        # lookup the data of the stocks and store it
        values = lookup(stock["symbol"])
        if values is None:
            flash("API Error: You have used all available credits for the month. Please contact the administrator of this website(https://github.com/EduBM7)")
            return apology("API Error: You have used all available credits for the month.")
        stock["name"] = values["name"]
        stock["currentPrice"] = values["price"]
        stock["currentTotal"] = stock["shares"] * stock["currentPrice"]
        if stock["bought_total"] != 0:
            # Get how much the stock appreciate compared to the bought price
            stock["appreciation"] = 100*((stock["currentTotal"] / stock["bought_total"]) - 1)
        else:
            stock["appreciation"] = 0

        # Will get the total value of the stocks at the bought price so we can see how much the total appreciate
        cost_basis += stock["bought_total"]

        # Display the current value of the stocks
        current_stock_cash += stock["currentTotal"]

    # Current balance of stocks total price + total cash
    cash_balance = current_stock_cash + cash[0]["cash"]

    # How much the stocks appreciate
    if cost_basis > 0:
        stocks_appreciation = 100*((current_stock_cash / cost_basis) - 1)
    else:
        stocks_appreciation = 0

    # How much your balance appreciate in relation to the money invested
    balance_appreciation = 100*((cash_balance / invested_cash) - 1)

    if request.method == "POST":
        buy_shares = request.form.get("shares-buy")
        sell_shares = request.form.get("shares-sell")

        if "form-buy" in request.form:

            cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])

            symbol = request.form.get("symbol-buy").upper()

            value = lookup(symbol)

            if buy_shares is None:
                flash("You must enter a positive whole number of shares.")
                return apology("You must enter a positive whole number of shares")
            try:
                buy_shares = int(buy_shares)

                if buy_shares < 0:
                    flash("You must enter a positive whole number of shares.")
                    return apology("You must enter a positive whole number of shares")
                else:
                    total = buy_shares * float(value["price"])

                    cash = cash[0]["cash"]

                    if cash < total:
                        flash("You have insufficient funds in your account.")
                        return apology("You have insufficient funds in your account")

                    else:
                        db.execute("UPDATE users SET cash = cash - ? WHERE id = ?", total, session["user_id"])

                        db.execute("INSERT INTO stocks (session_id, symbol, shares, price, total, bought_total, operation) VALUES(?,?,?,?,?,?,?)",
                                   session["user_id"], symbol, buy_shares, value["price"], total, total, "Buy")
                        if buy_shares > 1:
                            flash(f"Bought {buy_shares} {symbol} shares for {usd(total)}!")
                            return redirect("/")

                        else:
                            flash(f"Bought {buy_shares} {symbol} share for {usd(total)}!")
                            return redirect("/")
            except ValueError:
                flash("You must enter a positive whole number of shares.")
                return apology("You must enter a positive whole number of shares")

        # Get the submit from the modal Sell
        elif "form-sell" in request.form:
            # get the symbol
            symbol = request.form.get("symbol-sell").upper()

            # lookup the values of the share
            value = lookup(symbol)

            # Select the amount of cash the user has
            cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])

            # Select the number of shares a given symbol has
            total_stocks = db.execute(
                "SELECT SUM(shares) as shares FROM stocks WHERE session_id = ? AND symbol = ?;", session["user_id"], symbol)

            if sell_shares is None:
                flash("You must enter a positive whole number of shares.")
                return apology("You must enter a positive whole number of shares")
            # try to parse shares as int() if not possible throw errors
            try:
                sell_shares = int(sell_shares)

                if sell_shares < 0:
                    flash("You must enter a positive whole number of shares.")
                    return apology("You must enter a positive whole number of shares")
                elif sell_shares > total_stocks[0]["shares"]:
                    flash("You dont have this number of shares available to sell.")
                    return apology("You dont have this number of shares available to sell")
                else:

                    # Initialize the counter to sell the shares in a FIFO manner
                    counter = sell_shares

                    # Initialize the total value that will be sold to get the FIFO list of bought shares
                    totalSelling = sell_shares * float(value["price"])

                    # Pass the totalSelling initial value to the earned variable to be calculated
                    earned = totalSelling

                    # Select the values from stocks to execute a FIFO sell of shares
                    stocks = db.execute(
                        "SELECT id, shares, symbol, price, sold, bought_total FROM stocks WHERE session_id = ? AND operation = 'Buy' AND symbol = ?;", session["user_id"], symbol)

                    # Sell shares and compare it to the bought shares in a FIFO way
                    for stock in stocks:
                        # store the remaining shares of that stock to be sold
                        remaining_shares = stock["shares"] - stock["sold"]

                        if remaining_shares == 0:
                            continue
                        elif counter <= remaining_shares:

                            # Current total price of shares sold
                            totalSold = counter * float(value["price"])

                            # amount of cash that the user have
                            money = cash[0]["cash"]

                            # amount of cash that the user earned with the transaction -> totalPriceSellShares - (cost_basisOfTheSell)
                            earned -= (counter * stock["price"])

                            # update user cash
                            db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", totalSold, session["user_id"])

                            # Update the stock sold values so we can know that stock was sold
                            db.execute("UPDATE stocks SET sold = sold + ? WHERE id = ? AND session_id = ?",
                                       counter, stock["id"], session["user_id"])

                            # Get the sold stock bought price so we can calculate the cost basis and appreciation of each stock
                            total_bought_price = counter * stock["price"]
                            db.execute("UPDATE stocks SET bought_total = bought_total - ? WHERE id = ? AND session_id = ?",
                                       total_bought_price, stock["id"], session["user_id"])

                            # insert the new operation values
                            db.execute("INSERT INTO stocks (session_id, symbol, shares, price, total, operation) VALUES(?,?,?,?,?,?)",
                                       session["user_id"], value["symbol"], -sell_shares, value["price"], -totalSelling, "Sell")

                            if earned >= 0:
                                flash(f"Sold for {usd(totalSold)}, you earned {usd(earned)} with this transaction!")
                                # break the loop cause we already sold all the shares the user wanted
                            else:
                                formatter = -earned
                                flash(f"Sold for {usd(totalSold)}, you lost {usd(formatter)} with this transaction!")
                                # break the loop cause we already sold all the shares the user wanted
                            break
                        else:
                            # Subtract counter from the remaining shares
                            counter -= remaining_shares
                            totalSold = remaining_shares * float(value["price"])

                            # Subtract from total the bought value of shares that will be sold
                            earned -= (remaining_shares * stock["price"])

                            money = cash[0]["cash"]

                            # update user cash
                            db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", totalSold, session["user_id"])

                            # Update the stock sold values so we can know that stock was sold
                            db.execute("UPDATE stocks SET sold = ? WHERE id = ? and session_id = ?",
                                       remaining_shares, stock["id"], session['user_id'])

                            # Get the sold stock bought price so we can calculate the cost basis and appreciation of each stock
                            total_bought_price = remaining_shares * stock["price"]
                            db.execute("UPDATE stocks SET bought_total = bought_total - ? WHERE id = ? AND session_id = ?",
                                       total_bought_price, stock["id"], session["user_id"])
                            continue
                    return redirect("/")
            except ValueError:
                flash("You must enter a positive whole number of shares.")
                return apology("You must enter a positive whole number of shares")

        # Code for when the add cash modal is raised
        elif "add-cash" in request.form:
            cash_amount = request.form.get("cash-amount")

            if cash_amount is None:
                flash("You must enter a positive whole number of cash.")
                return apology("You must enter a positive whole number of cash")
            try:
                cash_amount = int(cash_amount)

                if cash_amount < 0:
                    flash("You must enter a positive whole number of cash.")
                    return apology("You must enter a positive whole number of cash")

                else:
                    db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", cash_amount, session["user_id"])

                    # update invested cash to calculate how much our money appreciate
                    invested_cash += cash_amount
                    db.execute("UPDATE invest SET invested = invested + ? WHERE sessionid = ?", cash_amount, session["user_id"])

                    cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])

                    # update cash_balance and the balance appreciation
                    cash_balance = cash[0]["cash"] + current_stock_cash
                    balance_appreciation = 100*((cash_balance / invested_cash) - 1)

                    flash(f"{usd(cash_amount)} Added!")
                    return redirect("/")

            except ValueError:
                flash("You must enter a positive whole number of cash.")
                return apology("You must enter a positive whole number of cash")

        # Code for POST request made from our AJAX call
        else:
            current_stocks = db.execute(
                "SELECT SUM(shares) as shares, symbol, SUM(bought_total) as bought_total, SUM(total) as total FROM stocks WHERE session_id = ? GROUP BY symbol HAVING (SUM(shares)) > 0", session["user_id"])
            cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])
            invested = db.execute("SELECT invested FROM invest WHERE session_id = ? ", session["user_id"])

            invested_cash = invested[0]["invested"]

            # initialize our cost_basis
            cost_basis = 0

            current_stock_cash = 0

            for stock in current_stocks:
                # lookup the data of the stocks and store it
                values = lookup(stock["symbol"])
                stock["name"] = values["name"]
                stock["currentPrice"] = values["price"]
                stock["currentTotal"] = stock["shares"] * stock["currentPrice"]

                if stock["bought_total"] != 0:
                    # Get how much the stock appreciate compared to the bought price
                    stock["appreciation"] = 100*((stock["currentTotal"] / stock["bought_total"]) - 1)
                else:
                    stock["appreciation"] = 0

                # Will get the total value of the stocks at the bought price so we can see how much the total appreciate
                cost_basis += stock["bought_total"]

                # Display the current value of the stocks
                current_stock_cash += stock["currentTotal"]

            # Current balance of stocks total price + total cash
            cash_balance = current_stock_cash + cash[0]["cash"]

            # How much the stocks appreciate
            if cost_basis > 0:
                stocks_appreciation = 100*((current_stock_cash / cost_basis) - 1)
            else:
                stocks_appreciation = 0

            # How much your balance appreciate in relation to the money invested
            balance_appreciation = 100*((cash_balance / invested_cash) - 1)

            # our JSON data that will be send to the client
            data = {
                "blc_appr": balance_appreciation,
                "stck_appr": stocks_appreciation,
                "stck_balance": current_stock_cash,
                "cash_balance": cash_balance,
                "stocks": current_stocks,
            }

            return jsonify(data)

    # The initial values of the table
    else:
        return render_template("index.html", balance_appr=balance_appreciation, balance=cash_balance, stock_cash=current_stock_cash, stocks_appr=stocks_appreciation, stocks=stocks, cash=cash[0]["cash"])


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "POST":
        symbol = request.form.get("symbol").upper()
        values = lookup(symbol)
        shares = request.form.get("shares")
        cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])

        if values is None:
            flash("You must enter a valid stock symbol.")
            return apology("You must enter a valid stock symbol")
        try:
            shares = int(shares)
            if shares < 0:
                flash("You must enter a positive whole number of shares.")
                return apology("You must enter a positive whole number of shares")
            else:

                total = shares * float(values["price"])
                cash = cash[0]["cash"]

                if cash < total:

                    flash("You have insufficient funds in your account.")
                    return apology("You have insufficient funds in your account")

                else:
                    db.execute("UPDATE users SET cash = cash - ? WHERE id = ?", total, session["user_id"])

                    db.execute("INSERT INTO stocks (session_id, symbol, shares, price, total, bought_total, operation) VALUES(?,?,?,?,?,?,?)",
                               session["user_id"], symbol, shares, values["price"], total, total, "Buy")

                    return redirect("/")
        except ValueError:

            flash("You must enter a positive whole number of shares.")
            return apology("You must enter a positive whole number of shares")

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    stocks = db.execute("SELECT * FROM stocks WHERE session_id = ? ORDER BY id DESC", session["user_id"])

    for stock in stocks:
        stock["name"] = lookup(stock["symbol"])["name"]

    return render_template("history.html", stocks=stocks)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]
        session["username"] = rows[0]["username"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    if request.method == "POST":
        symbol = request.form.get("symbol").upper()
        values = lookup(symbol)
        if values is None:
            flash("You must enter a valid stock symbol")
            return apology("You must enter a valid stock symbol")
        else:
            return render_template("quoted.html", symbol=symbol, name=values["name"], price=values["price"])
    return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        confirm_password = request.form.get("confirmation")

        check_username = db.execute("SELECT * FROM users WHERE username = ?", username)

        if not username:
            flash("You must provide a username.")
            return apology("You must provide a username")

        elif len(check_username) != 0:
            flash("This username already exist.")
            return apology("This username already exist")

        # Ensure password was submitted
        elif not password:
            flash("You must provide a password.")
            return apology("You must provide a password")

        # Ensure confirmation password was submitted
        elif not confirm_password:
            flash("You must confirm your password.")
            return apology("You must confirm your password")

        # Ensure passwords match
        elif not password == confirm_password:
            flash("Passwords does not match.")
            return apology("Passwords does not match")

        else:
            hash_password = generate_password_hash(password, method="pbkdf2:sha256", salt_length=16)
            db.execute("INSERT INTO users (username, hash) VALUES(?, ?)", username, hash_password)

            return redirect("/")
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    stocks = db.execute(
        "SELECT SUM(shares) as shares, symbol FROM stocks WHERE session_id = ? GROUP BY symbol HAVING (SUM(shares)) > 0", session["user_id"])

    if request.method == "POST":
        symbol = request.form.get("symbol").upper()
        values_sell = lookup(symbol)
        shares_sell = request.form.get("shares")

        # Select the number of shares you have from a given company
        total_stocks = db.execute(
            "SELECT SUM(shares) as shares FROM stocks WHERE session_id = ? AND symbol = ?;", session["user_id"], symbol)

        # Select the user current cash
        cash = db.execute("SELECT cash FROM users WHERE id = ? ", session["user_id"])

        if symbol == "stocks":
            flash("You must select a stock to sell.")
            return apology("You must select a stock to sell")

        elif shares_sell is None:
            flash("You must enter a positive whole number of shares.")
            return apology("You must enter a positive whole number of shares")

        else:
            try:
                shares_sell = int(shares_sell)

                if shares_sell < 0:
                    flash("You must enter a positive whole number of shares.")
                    return apology("You must enter a positive whole number of shares")
                elif shares_sell > total_stocks[0]["shares"]:
                    flash("You dont have this number of shares available to sell.")
                    return apology("You dont have this number of shares available to sell")
                else:
                    # Set the counter variable to count the stocks sold and be able to execute the FIFO sell
                    counter = shares_sell

                    # Select the values from stocks to execute a FIFO sell of shares
                    stocks = db.execute(
                        "SELECT id, shares, symbol, price, sold FROM stocks WHERE session_id = ? AND operation = 'Buy' AND symbol = ?;", session["user_id"], symbol)

                    totalSelling = shares_sell * float(values_sell["price"])

                    # Pass the totalSelling initial value to the earned variable to be calculated
                    earned = totalSelling

                    # Loop through stocks to sell shares and compare it to the bought shares in a FIFO way
                    for stock in stocks:

                        # store the remaining shares of that stock to be sold
                        remaining_shares = stock["shares"] - stock["sold"]

                        if remaining_shares == 0:
                            continue
                        elif counter <= remaining_shares:
                            # Current total price of shares sold

                            # amount of cash that the user have
                            cash = cash[0]["cash"]

                            # Total Sold in for this stock
                            totalSold = counter * float(values_sell["price"])

                            # Subtract from earned the bought value of shares that will be sold. Earned is the amount of cash that the user earned with the transaction
                            earned -= (counter * stock["price"])

                            # update user cash
                            db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", totalSold, session["user_id"])

                            # Update the stock sold values so we can know that stock was sold
                            db.execute("UPDATE stocks SET sold = ? WHERE id = ? AND session_id = ?",
                                       counter, stock["id"], session['user_id'])

                            # Get the sold stock bought price so we can calculate the cost basis and appreciation of each stock
                            total_bought_price = remaining_shares * stock["price"]
                            db.execute("UPDATE stocks SET bought_total = bought_total - ? WHERE id = ? AND session_id = ?",
                                       total_bought_price, stock["id"], session["user_id"])

                            # insert the new operation values
                            db.execute("INSERT INTO stocks (session_id, symbol, shares, price, total, operation) VALUES(?,?,?,?,?,?)",
                                       session["user_id"], values_sell["symbol"], -shares_sell, values_sell["price"], -totalSelling, "Sell")

                            if earned >= 0:
                                flash(f"Sold for {usd(totalSold)}, you earned {usd(earned)} with this transaction!")
                                # break the loop cause we already sold all the shares the user wanted
                            else:
                                formatter = -earned
                                flash(f"Sold for {usd(totalSold)}, you lost {usd(formatter)} with this transaction!")
                                # break the loop cause we already sold all the shares the user wanted
                            break
                        else:
                            counter -= remaining_shares

                            totalSold = remaining_shares * float(values_sell["price"])

                            # Subtract from total the bought value of shares that will be sold
                            earned -= (remaining_shares * stock["price"])

                            cash = cash[0]["cash"]

                            # update user cash
                            db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", totalSold, session["user_id"])

                            # Update the stock sold values so we can know that stock was sold
                            db.execute("UPDATE stocks SET sold = ? WHERE id = ? AND session_id = ?",
                                       remaining_shares, stock["id"], session["user_id"])

                            # Get the sold stock bought price so we can calculate the cost basis and appreciation of each stock
                            total_bought_price = remaining_shares * stock["price"]
                            db.execute("UPDATE stocks SET bought_total = bought_total - ? WHERE id = ? AND session_id = ?",
                                       total_bought_price, stock["id"], session["user_id"])
                            continue
                    return redirect('/')
            except ValueError:
                flash("You must enter a positive whole number of shares.")
                return apology("You must enter a positive whole number of shares")

    else:
        return render_template("sell.html", stocks=stocks)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)
