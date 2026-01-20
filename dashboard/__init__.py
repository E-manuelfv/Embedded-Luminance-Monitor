from sys import prefix
from flask import Flask
import secrets

class MyDashboard(Flask):
    def __init__(self, config_object=None, **kwargs):
        self.app = Flask(__name__, template_folder='views/templates', static_folder='views/static', **kwargs)
        self.app.config['SECRET_KEY'] = secrets.token_hex(16)
        
        if config_object:
            self.app.config.from_object(config_object)

        from .controllers.main import main_blueprint

        self.app.register_blueprint(main_blueprint)

    def run(self):
        self.app.run(debug=True)