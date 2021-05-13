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
                <h1 style="color:blue;text-align:center;">Tuple Tunes: Unlock Your Musical Genius</h1>
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
            toReturn += parse_game(c, int(game_id[0])) 

        # (0, 'BBB', 9, 37, 'in_game', 0, 0, '2021-05-12 15:27:20.895961', '2021-05-12 15:27:20.895971')

        return toReturn

def parse_game(c, game_id):

    #get game info
    game_info = c.execute('''SELECT host, key, tempo FROM games WHERE rowid = ?;''', (game_id,)).fetchone()

    #get players
    player_info = c.execute('''SELECT username FROM players WHERE game_id = ?;''', (game_id,))

    player_names = [player[0] for player in player_info]

    #get measures
    measures_info = c.execute('''SELECT measure_number, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16 FROM measures WHERE game_id = ? ORDER BY measure_number ASC''', (game_id,)).fetchall()

    measure_str = ""
    for measure in measures_info:
        measure_str += f"Measure #{measure[0]}: "
        measure_str += ", ".join([str(val) for val in measure[1:]])
        measure_str += "<br>"

    host_name, key, tempo = game_info

    return f'''
    <div style="font-style:italic;">
        <hr>
        <h1>Host Name: {host_name}</h1>
        <h2>Key: {key}</h2>
        <h2>Tempo: {tempo}</h2>
        <h3>Players: {', '.join(player_names)} <h3>
        <h3>{measure_str} <h3>
    </div>
    '''
