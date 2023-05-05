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
import random
import time
import uuid
import datetime

from py4web import action, request, abort, redirect, URL, Field, HTTP
from py4web.utils.form import Form, FormStyleBulma
from py4web.utils.url_signer import URLSigner

from yatl.helpers import A
from . common import db, session, T, cache, auth, signed_url


url_signer = URLSigner(session)

# The auth.user below forces login.
@action('index')
@action.uses(auth.user, url_signer, session, db, 'index.html')
def index():
    return dict(
        # This is an example of a signed URL for the callback.
        # See the index.html template for how this is passed to the javascript.
		posts_url = URL('posts', signer=url_signer),
        delete_url = URL('delete_post', signer=url_signer),
        color_url = URL('update_color', signer=url_signer),
        favorite_url = URL('change_fav', signer=url_signer),
        user_email = auth.current_user.get('email'),
        author_name = auth.current_user.get('first_name') + " " + auth.current_user.get('last_name')
    )

@action('posts', method="GET")
@action.uses(db, auth.user, session, url_signer.verify())
def get_posts():
    # You can use this shortcut for testing at the very beginning.
    favoritedData=db((db.post.email==auth.current_user.get("email")) & (db.post.favorite==True) ).select(db.post.ALL, orderby = ~db.post.last_update).as_list()
    postData=db((db.post.email==auth.current_user.get("email"))& (db.post.favorite==False)).select(orderby = ~db.post.last_update).as_list()
    finalData=[]

    for post in favoritedData:
    	finalData.append(post)

    for post in postData:
    	finalData.append(post)

    return dict(posts=finalData)

@action('posts',  method="POST")
@action.uses(db, auth.user, session, url_signer.verify())  # etc.  Put here what you need.
def save_post():
    
    id = request.json.get('id') # Note: id can be none.
    content = request.json.get('content')
    incom_title = request.json.get('title')
    # If id is None, this means that this is a new post that needs to be
    # inserted.  If id is not None, then this is an update.
    if id is None:
    	id=db.post.insert(content=content, title=incom_title)
    else:
    	# print("edit post!")
    	db(db.post.id == id).update(content = content, title = incom_title, last_update=datetime.datetime.utcnow())
    # print(id, content,is_reply, sep="--")
    return dict(content=content, id=id, title=incom_title)

@action('delete_post',  method="POST")
@action.uses(db, auth.user, session, url_signer.verify())
def delete_post():
    db((db.post.email == auth.current_user.get("email")) &
       (db.post.id == request.json.get('id'))).delete()
    return "ok"

@action('update_color',  method="POST")
@action.uses(db, auth.user, session, url_signer.verify())
def update_color():
    db((db.post.email == auth.current_user.get("email")) &
       (db.post.id == request.json.get('id'))).update(color=request.json.get('new_color'))
    return "ok"

@action('change_fav',  method="POST")
@action.uses(db, auth.user, session, url_signer.verify())
def change_fav():
    db((db.post.email == auth.current_user.get("email")) &
       (db.post.id == request.json.get('id'))).update(favorite=request.json.get('new_val'))
    return "ok"

@action('delete_all_posts')
@action.uses(db)
def delete_all_posts():
    """This should be removed before you use the app in production!"""
    db(db.post).delete()
    return "ok"
