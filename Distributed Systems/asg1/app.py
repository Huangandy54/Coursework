# app.py - asg1 
# ahuang44
# basic features of REST webserver as specified in pdf
from flask import Flask, request
from flask_api import status

app = Flask(__name__)

@app.route('/hello', methods=['GET'])
def hello_world():
    return 'Hello, world!',status.HTTP_200_OK

@app.route('/hello', methods=['POST'])
def unsupported():
    return 'This method is unsupported.',status.HTTP_405_METHOD_NOT_ALLOWED

@app.route('/check', methods=['GET'])
def confirmation():
    return 'GET message received', status.HTTP_200_OK


@app.route('/check', methods=['POST'])
def confirmationMsgRec():
    if 'msg' in request.args:
        return 'POST message received: '+request.args['msg'], status.HTTP_200_OK

    else:
        return 'This method is unsupported.', status.HTTP_405_METHOD_NOT_ALLOWED



if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port= 8081)