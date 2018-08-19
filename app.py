from flask import Flask, Response, request
from flask_cors import CORS
from subprocess import check_output, PIPE, Popen
from threading import Thread
from multiprocessing import Process

app = Flask(__name__)
CORS(app)


@app.route('/<username>/<password>', methods=['GET'])
def test(username, password):
    print('___request zaprimljen___')
    print(username, password)

    sudo_password = '1950torca'
    command = 'sudo gnome-screensaver-command -l'.split()

    p = Popen(['sudo', '-S'] + command, stdin=PIPE, stderr=PIPE,
              universal_newlines=True)
    sudo_prompt = p.communicate(sudo_password + '\n')[1]

    return Response('Zaprimljeno korisnicko ime i sifra {} {}'.format(username, password), 200)


@app.route('/auth', methods=['POST'])
def method_name():
    username = request.form['username']
    password = request.form['password']
    return Response('Zaprimljeno korisnicko ime i sifra', 200)


if __name__ == '__main__':
    def start():
        app.run(host='0.0.0.0')
    thread = Process(target=start)
    thread.start()
    print("THREAD RUNING")
