# model.py
from flask_wtf import FlaskForm
from wtforms import SubmitField, StringField, TextAreaField
from wtforms.validators import InputRequired, Length, Email




class ContactForm(FlaskForm):
    first_name = StringField('First Name', 
                    validators=[InputRequired('Please enter your first name.'), Length(min=2, max=25)])
    
    last_name =  StringField('Last Name', 
                    validators=[InputRequired('Please enter your last name.'), Length(min=2, max=30)])
    
    email = StringField('Email Address', 
                    validators=[InputRequired('Please enter your email address.'), Length(min=6, max=120), Email('Please enter a valid email address.')])
    
    text = TextAreaField('Message', 
                    validators=[InputRequired('Please enter a message with 800 characters max.'), Length(min=2, max=800)])
    submit = SubmitField('Submit')


