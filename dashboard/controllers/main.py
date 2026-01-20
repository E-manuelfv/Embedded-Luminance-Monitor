from flask import Blueprint, render_template, flash, redirect, url_for
from dashboard.models.supabase_client import supabase
from dashboard.controllers.services import counter

main_blueprint = Blueprint('main', __name__, url_prefix='/')

@main_blueprint.route('/')
def index():
    try:
        response = counter()
        if response and response.data:
            clicks_data = response.data
        else:
            clicks_data = []
            flash("Nenhum dado de cliques encontrado.", "warning")
    except Exception as e:
        clicks_data = []
        flash(f"Erro ao carregar dados: {e}", "danger")

    return render_template('index.html', clicks=clicks_data)