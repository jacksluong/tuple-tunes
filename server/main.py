# import sys
# sys.path.append('__HOME__/server')

import sqlite3

html_file = '/var/jail/home/team59/server/index.html'
moosic_db = '/var/jail/home/team59/moosic6.db'

def request_handler(request):
    """
    RENDER WEB PAGE
    """ 
    if request['method'] == "GET":
        game_data = get_game_data()
        return f'''
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8" />
                <meta http-equiv="X-UA-Compatible" content="IE=edge" />
                <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                <title>Tuple Tunes</title>
            </head>
            <body>
                <h1>Web Page For Tuple Tunes (MOST to LEAST RECENT)</h1>
                {game_data}
            </body>
            </html>
        '''
    else:
        return "NO POST REQUESTS ALLOWED"

def get_game_data():
    with sqlite3.connect(moosic_db) as c: 

        #get game data
        list_ids = c.execute('''SELECT rowid FROM games ORDER BY time DESC''').fetchall()

        #get the game info of each game
        toReturn = ""
        for game_id in list_ids:
            toReturn += parse_game(c, game_id[0]) 

        # (0, 'BBB', 9, 37, 'in_game', 0, 0, '2021-05-12 15:27:20.895961', '2021-05-12 15:27:20.895971')

        return toReturn

def parse_game(c, game_id):
    game_info = c.execute('''SELECT host, key, tempo FROM games WHERE rowid = ?;''', (int(game_id),)).fetchone()

    host_name, key, tempo = game_info

    return f'''
    <h1>Host Name: {host_name}</h1>
    <h2>Key: {key}</h2>
    <h2>Tempo: {tempo}</h2>
    '''
