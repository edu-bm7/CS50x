from flask import Flask, app, flash, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_session import Session
from flask_mail import Mail, Message
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from model import ContactForm
import os

application = Flask(__name__)
application.config.from_mapping(
    SECRET_KEY=b'\x14S\x93\x15a3s\x1a+w\x9f\x99$\x8f\xb9DJ\x87\x0b\x0b\xd8c\xf3\x1d')

application.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure SQLAlchemy Database
application.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///contact.db'
db = SQLAlchemy(application)

# Configure the email
application.config["MAIL_DEFAULT_SENDER"] = os.getenv("MAIL_DEFAULT_SENDER")
application.config["MAIL_PASSWORD"] = os.getenv("MAIL_PASSWORD")
application.config["MAIL_PORT"] = 587
application.config["MAIL_SERVER"] = "smtp.gmail.com"
application.config["MAIL_USE_TLS"] = True
application.config["MAIL_USERNAME"] = os.getenv("MAIL_USERNAME")
mail = Mail(application)

# Configure session to use filesystem (instead of signed cookies)
application.config["SESSION_FILE_DIR"] = mkdtemp()
application.config["SESSION_PERMANENT"] = False
application.config["SESSION_TYPE"] = "filesystem"
Session(application)


class Contact(db.Model):
    __tablename__ = 'contact'

    id = db.Column(db.Integer, primary_key=True)
    first_name = db.Column(db.String(25), nullable=False)
    last_name = db.Column(db.String(30), nullable=False)
    email = db.Column(db.String(120), nullable=False)
    text = db.Column(db.String(800), nullable=False)

db.create_all()

# Ensure responses aren't cached
@application.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@application.route('/')
def index():
    return render_template('index.html')


@application.route('/about')
def about():
    return render_template('about.html')


@application.route('/contact', methods=['GET', 'POST'])
def contact():
    form = ContactForm()
    if form.validate_on_submit():
        f_name = form.first_name.data
        l_name = form.last_name.data
        email_address = form.email.data
        text_message = form.text.data

        message = Contact(first_name=f_name, last_name=l_name, email=email_address, text=text_message)
        db.session.add(message)
        db.session.commit()
        email_msg = f"First Name: {f_name}\nLast Name: {l_name}\nEmail: {email_address}\nMessage: {text_message}"
        msg = Message("CS50x FinalProject!",body=email_msg, recipients=["bmoraes.eduardo@gmail.com"])
        mail.send(msg)
        flash('Your message was sent!')
        return render_template('contact.html', form=form)
    return render_template('contact.html', form=form)


@application.route('/source')
def source():
    return render_template('source.html')


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    flash(f"Error:{e.name}, Code:{e.code}")
    return render_template('index.html')


# Listen for errors
for code in default_exceptions:
    application.errorhandler(code)

if __name__ == "__main__":
    application.run()