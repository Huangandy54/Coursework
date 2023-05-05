"""
This file defines actions, i.e. functions the URLs are mapped into
The @action(path) decorator exposed the function at URL:

    http://127.0.0.1:8000/{app_name}/{path}

If app_name == '_default' then simply

    http://127.0.0.1:8000/{path}

If path == 'index' it can be omitted:

    http://127.0.0.1:8000/

The path follows the bottlepy syntax.

@action.uses('generic.html')  indicates that the action uses the generic.html template
@action.uses(session)         indicates that the action uses the session
@action.uses(db)              indicates that the action uses the db
@action.uses(T)               indicates that the action uses the i18n & pluralization
@action.uses(auth.user)       indicates that the action requires a logged in user
@action.uses(auth)            indicates that the action requires the auth object

session, db, T, auth, and tempates are examples of Fixtures.
Warning: Fixtures MUST be declared with @action.uses({fixtures}) else your app will result in undefined behavior
"""

import uuid

from py4web import action, request, abort, redirect, URL, Field
from py4web.utils.form import Form, FormStyleBulma
from py4web.utils.url_signer import URLSigner
from pydal.validators import *

from yatl.helpers import A
from . common import db, session, T, cache, auth, signed_url


url_signer = URLSigner(session)

# The auth.user below forces login.
@action('index', method='GET')
@action.uses('index.html', auth.user,session,db)
def index():
	#passes user variable to allow for signout button to display while signed in.
	user=auth.current_user
	#query that selects the contacts under the email of the current logged in user
	rows = db(db.person.user_email==user.get('email')).select().as_list()
	for row in rows:
		
		s=db(db.phone.owner_id==row.get('id')).select(db.phone.phone_number,db.phone.phone_name)
		
		row["phone_number"]=s
		
		pass
	return dict(rows=rows, url_signer=url_signer, user=user)


@action('add_contact', method=['GET', 'POST'])
@action.uses('contact_form.html', auth.user, session, db)
def add_contact():
    form = Form(db.person, csrf_session=session, formstyle=FormStyleBulma)
    if form.accepted:
        # We always want POST requests to be redirected as GETs.
        redirect(URL('index'))
    return dict(form=form)

@action('edit_contact/<contact_id>', method=['GET', 'POST'])
@action.uses('contact_form.html', auth.user, session, db)
def edit_contact(contact_id=None):
    
    p = db.person[contact_id]
    pCreator=p.user_email
    #checks if actually a entry
    if p is None:
        redirect(URL('index'))
    #checks if current user is editing their own contact
    if pCreator!=auth.current_user.get('email'):
    	redirect(URL('index'))
    
    form = Form(db.person, record=p, deletable=False, csrf_session=session, formstyle=FormStyleBulma)
    if form.accepted:
        # We always want POST requests to be redirected as GETs.
        redirect(URL('index'))
    return dict(form=form)


@action('delete_contact/<contact_id>', method=['GET','POST'])
@action.uses('index.html',auth.user, session, db, url_signer.verify())
def delete_contact(contact_id=None):
    p = db.person[contact_id]
    pCreator=p.user_email
    #check if actually a entry
    if p is None:
        redirect(URL('index'))
    #if user is deleting a contact that isnt theirs
    if pCreator!=auth.current_user.get('email'):
    	redirect(URL('index'))
    else:
        db(db.person.id == contact_id).delete()
        redirect(URL('index'))


@action('edit_phones/<contact_id>', method=['GET', 'POST'])
@action.uses('edit_phones.html', auth.user, session, db)
def edit_phones(contact_id=None):
	
    #passes user variable to allow for signout button to display while signed in.
	user=auth.current_user
	#pulls data from person table
	contactInfo=db(db.person.id==contact_id).select()
	
	if auth.current_user.get('email')!=contactInfo[0].user_email:
		redirect(URL('index'))
	#query that selects the contacts under the email of the current logged in user
	rows = db((db.person.user_email==user.get('email')) & (db.person.id==contact_id) & (db.phone.owner_id==contact_id)).select().as_list()
	
	
	return dict(rows=rows,contactInfo=contactInfo, url_signer=url_signer, user=user)


@action('add_phone/<contact_id>', method=['GET', 'POST'])
@action.uses('phone_form.html', auth.user, session, db)
def add_phone(contact_id=None):
	contactInfo=db(db.person.id==contact_id).select()
	
	if auth.current_user.get('email')!=contactInfo[0].user_email:
		redirect(URL('index'))
		
	form = Form([Field('phone_number', requires=IS_NOT_EMPTY()),Field('phone_name',requires=IS_NOT_EMPTY())],csrf_session=session,formstyle=FormStyleBulma)
	if form.accepted:
		# We always want POST requests to be redirected as GETs.
		
		db.phone.insert(phone_number=form.vars["phone_number"], phone_name=form.vars["phone_name"], owner_id=contact_id)
		redirect(URL('edit_phones',contact_id))
	return dict(form=form, contactInfo=contactInfo)



@action('edit_phone/<contact_id>/<phone_id>', method=['GET', 'POST'])
@action.uses('phone_form.html', auth.user, session, db)
def edit_phone(contact_id=None, phone_id=None):
	contactInfo=db(db.person.id==contact_id).select()
	
	if auth.current_user.get('email')!=contactInfo[0].user_email:
		redirect(URL('index'))
	p = db.phone[phone_id]
	
	form = Form([Field('phone_number',requires=IS_NOT_EMPTY()),Field('phone_name',requires=IS_NOT_EMPTY())],record=dict(phone_number=p.phone_number, phone_name=p.phone_name),deletable=False, csrf_session=session,formstyle=FormStyleBulma)
	if form.accepted:
		
		#modeled after documentation on update
		myset = db(db.phone.id==phone_id)
		myset.update(phone_number=form.vars["phone_number"], phone_name=form.vars["phone_name"], owner_id=contact_id)
		redirect(URL('edit_phones',contact_id))
	return dict(form=form,contactInfo=contactInfo)

@action('delete_phone/<contact_id>/<phone_id>', method=['GET','POST'])
@action.uses('edit_phones.html',auth.user, session, db, url_signer.verify())
def delete_phone(contact_id=None,phone_id=None):
    p = db.phone[phone_id]
    contactInfo=db(db.person.id==contact_id).select()
    
    #check if actually a entry
    if p is None:
        redirect(URL('edit_phones',contact_id))
    #if user is deleting a contact that isnt theirs
    if contactInfo[0].user_email!=auth.current_user.get('email'):
    	redirect(URL('edit_phones',contact_id))
    else:
        db(db.phone.id == phone_id).delete()
        redirect(URL('edit_phones',contact_id))

