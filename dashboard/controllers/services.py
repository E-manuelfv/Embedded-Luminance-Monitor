from dashboard.models.supabase_client import supabase 

CLICKS = 'clicks_counter' 

def counter():
    try:
        response = supabase.table(CLICKS).select('*').execute()
        
        return response
    except Exception as e:
        print(f"Erro ao contar cliques no Supabase: {e}")
        return None