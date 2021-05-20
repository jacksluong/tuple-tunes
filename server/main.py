

import sqlite3
archive = '/var/jail/home/team59/archive.db'

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
    with sqlite3.connect(archive) as c: 

        #get game data
        list_ids = c.execute('''SELECT rowid FROM games''').fetchall()

        #get the game info of each game
        toReturn = ""
        for game_id in list_ids:
            toReturn += parse_game(c, int(game_id[0])) 

        return toReturn

def parse_game(c, game_id):

    #get game info
    game_info = c.execute('''SELECT host, key, tempo, song FROM games WHERE rowid = ?;''', (game_id,)).fetchone()

    #get players
    player_info = c.execute('''SELECT username FROM players WHERE game_id = ?;''', (game_id,))
    player_names = [player[0] for player in player_info]


    host_name, key, tempo, song = game_info

    return f'''
    <div style="font-style:italic;">
        <hr>
        <h2>Host Name: {host_name}</h2>
        <h2>Key: {key}</h2>
        <h2>Tempo: {tempo}</h2>
        <h3>Players: {', '.join(player_names)} <h3>
        <h3>Song: {song} <h3>
    </div>
    '''
