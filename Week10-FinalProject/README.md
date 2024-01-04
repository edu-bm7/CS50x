# Glassmorphism Full Stack Website
## Video Demo: https://youtu.be/vLrBGoPEFGA
## Description
My project is a Full Stack Glassmorphism Website, with the back-end in Flask(Python) and the front-end in HTML, CSS and JavaScript. I also use SQLite3 as the Database so I could capture leads for marketing porpouses.

## I'm Not a Web-Designer
For this project I made a Glassmorphism Website based on the [Online Tutorials](https://www.youtube.com/watch?v=zSg4_d6Qhzc) glassmorphism landpage video. I did not copy his code, I also created the layout for the other pages in Figma and I added a lot of funtionality to the website.

## Scroll
For the scrolling effects in this container I used GSAP Library with ScrollTrigger Plugin.

## Why Not Bootstrap
I didn't want to use a CSS framework(Like Bootstrap or Tailwind) so I could challenge myself and do everything from scratch, also to not rewrite the whole UI kits available in the Frameworks.

## Explaining the Code


Let's start with our [application.py](project/application.py) file.

## Configuration of the application


Right at the top of the file, you'll see all the modules required to our website work.

Bellow that we see that we have to set a `SECRET_KEY` for [Flask-SQLAlchemy](https://github.com/pallets/flask-sqlalchemy) and [Flask-WTForms](https://github.com/wtforms/flask-wtf) to work.

Further down we configure our database and our email to use environment variables for the email configuration.

We then configure our session to use the filesystem instead of signed cookies.

After all these pre-cofiguration, we create our database with the code:

```
class Contact(db.Model):
    __tablename__ = 'contact'

    id = db.Column(db.Integer, primary_key=True)
    first_name = db.Column(db.String(25), nullable=False)
    last_name = db.Column(db.String(30), nullable=False)
    email = db.Column(db.String(120), nullable=False)
    text = db.Column(db.String(800), nullable=False)

db.create_all()
```

And to ensure that the reponses aren't cached we use this configuration:
```
@application.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response
```

## The Routes

There is nothing special about our routes, except in the contact page route.

There we first initiate our form.
```
form = COntactForm()
```

Then, if the form was validated, we get the data and store it in the database:
```
message = Contact(first_name=f_name, last_name=l_name, email=email_address, text=text_message)
        db.session.add(message)
        db.session.commit()
```

After storing the data at the database, we create the body of our email message and send it to the email configured before. We then flash a message to the user that the message was sent:
```
email_msg = f"First Name: {f_name}\nLast Name: {l_name}\nEmail: {email_address}\nMessage: {text_message}"
        msg = Message("CS50x FinalProject!",body=email_msg, recipients=["bmoraes.eduardo@gmail.com"])
        mail.send(msg)
        flash('Your message was sent!')
        return render_template('contact.html', form=form)
```

We also have our [model.py](project/model.py) file. Where the WTForms class is created and inserted the required validators.

Now, let's have a look at our **Templates**


## Templates

### **Layout**

Let's check out our [Layout](project/templates/layout.html) page.

Our `<head>` tag has everything we need for our website, that's includes google fonts, the GSAP library and the ScrollTrigger plugin.

Notice that on my `<body>` tag I have a `onload` function call. That's a message, sugesting the user to use either Chrome, Opera or Safari, due to Backdrop-Filter blur incompatibilities.

At our `<header>` tag we got our flashes messages, then we got our `<nav>` tag inside our header.
Notice that I have dinamicly choose how to change classes in the nav bar, so we can change the active page. We could do this with javascript. I just found that way easier.

We then have finally, our main block, in which each template will have its own.

The others templates doesn't have anything special, just normal html, with
the exception of the [About](project/templates/about.html) page.

### **About**

That's our [About](project/templates/about.html) page.

From the HTML, the only thing to pay attention is the `id="#trigger"` and the classes `class="pinTrigger"`, `class="title"` and `class="text-scrll"`, they will be used by GSAP to trigger the scroll events and animate it.

So let's check out the `<script>` tag where the GSAP code with the ScrollTrigger plugin is written.

Right at the first line, for those who never used GSAP plugins before is the function to register the ScrollTrigger Plugin.

The GSAP with ScrollTrigger Plugin enable us to easily animate elements when they enter the viewport with absolutely freedom.

First, we set our variables. `title` is a list with all the titles that will serve as **Trigger Element** and as a **Target Element** for GSAP animations, `t_len` is the length of that list. Then we have our second **Target Element** list `text` and finnaly our `pin` that will be our **Targer Pin** list, meaning, that we gonna *Pin* the elements when that class enters the viewport.

Then, what I did was to make a 3 loops to make 3 types of animation and alternate it between the list of **Target Elements**

Inside the loop, we first set our `triggerElement` that will be our `title[i]`, then we set our `targetElement1` to be the same as the **Trigger Element**, because we want to animate it too. then we set our `targetElement2` to be our `text[i]` and finally we set our `targetPin` to be our `pin[i]`.

After setting our variables, we can create our timeline as in the code bellow:

```
let tl = gsap.timeline({
            scrollTrigger: {
               trigger: triggerElement,
               toggleActions: "play reverse play reverse",
               scroller: '#about-text',
               pin: targetPin,
               scrub: true,
               start: 'top 25%',
               end: '+=1000px'
            }
         });
```

The `toggleActions` sets how the animation will behave in 4 different toggles - onEnter, onLeave, onEnterBack, and onLeaveBack, in that order.

We then can animate our **Target Elements**, for these I choose to animate from what state and position the elements would be so for this we use `tl.from()`(remember that `tl` is `gsap.timeline()`) as in the code:

```
tl.from(targetElement1, {
            x: '-100%',
            duration: 1,
            ease: 'elastic.out'
         })

tl.from(targetElement2, {
            y: '300%',
            opacity: '0',
            duration: 2,
            ease: 'power3.out'
         }, '<')
```

The `'<'` means that the animation will be performed after the first animation.





