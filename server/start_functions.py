import sqlite3
import datetime
import random
from in_game_functions import *

moosic_db = '/var/jail/home/team59/moosic7.db'

ROOM_CAPACITY = 10


def create_game(host, key, tempo):
    '''creates a new game row
    returns the game code'''
    game_code = 0  # default

    with sqlite3.connect(moosic_db) as c:
        # create GAMES TABLE
        c.execute(
            '''CREATE TABLE IF NOT EXISTS games(game_code int, host text, key int, tempo int, game_status text, turn int, measure int, disconnect_check timestamp, time timestamp);''')

        # CREATE PLAYERS TABLE
        c.execute(
            '''CREATE TABLE IF NOT EXISTS players(game_id real, username text, last_ping timestamp, entry_time timestamp);''')

        #get a random game code
        codes = c.execute('''SELECT game_code FROM games;''').fetchall()
        if len(codes) != 0:
            code_set = {code[0] for code in codes}
            game_code = random.choice(list(set(range(1000)) - code_set))

        #create new entry in games database
        c.execute('''INSERT INTO games VALUES (?,?,?,?,?,?,?,?,?);''',
                  (game_code, host, key, tempo, 'start', 0, 0, datetime.datetime.now(), datetime.datetime.now()))

        # get game_id of last inserted game
        game_id = c.execute('''SELECT rowid FROM games ORDER BY time DESC;''').fetchone()[0]

        # create a row in players
        c.execute('''INSERT INTO players VALUES (?,?,?,?);''',
                  (game_id, host, datetime.datetime.now(), datetime.datetime.now()))

    # turn game code into 3 length string
    game_code_string = '0' * (3 - len(str(game_code))) + str(game_code)

    return f"{game_code_string}&{game_id}"


def join_game(username, game_code):
    """
    Allow player with given username to join a game with the given game_code
    """

    with sqlite3.connect(moosic_db) as c:

        # get game_id and game_status of game
        game = c.execute('''SELECT rowid, game_status FROM games WHERE game_code = ?;''', (game_code,)).fetchone()

        # if game room not found
        if not game:
            # return "No Game Room Found"
            return "0"

        game_id, game_status = game

        # check if game is in start state
        if game_status == "start":
            # check num of players
            players = c.execute('''SELECT username FROM players WHERE game_id = ?;''', (game_id,)).fetchall()
            num_players = len(players)

            # check if room is still open to players
            if num_players < ROOM_CAPACITY:

                # insert player into database
                c.execute('''INSERT INTO players VALUES (?,?,?,?);''',
                          (game_id, username, datetime.datetime.now(), datetime.datetime.now()))
                
                key, tempo = c.execute('''SELECT key, tempo FROM games WHERE game_code = ?;''', (game_code,)).fetchone()

                # successful!
                return f"3&{game_id}&{key}&{tempo}"

            else:
                # room capacity hit
                return "1"

        else:
            # game is in progress or ended
            return "2"


def fetch_game_status(game_id):
    """
    Given a game_id, return the information about the status of the game
    """

    with sqlite3.connect(moosic_db) as c:

        # get the status of the game
        try:
            status = c.execute('''SELECT game_status FROM games WHERE rowid = ?;''', (game_id,)).fetchone()[0]
        except Exception as e:

            # game room not found
            return "0"

        # still in start state (waiting) state
        if status == "start":
            players = c.execute('''SELECT username FROM players WHERE game_id = ?;''', (game_id,)).fetchall()
            num_players = len(players)
            player_names = [val[0] for val in players]

            return f"1&{num_players}&{','.join(player_names)}"

        elif status == "in_game":
            players = c.execute('''SELECT username FROM players WHERE game_id = ?;''', (game_id,)).fetchall()
            num_players = len(players)
            player_names = [val[0] for val in players]

            for player_name in player_names:
                c.execute('''UPDATE players SET last_ping = ? WHERE username = ?;''',(datetime.datetime.now(), player_name))
                
            return "2"

        elif status == "ended":
            return "3"

def start_game(game_id):
    """
    Start the game with the give game id
    """  
    with sqlite3.connect(moosic_db) as c:

        # get status of game
        try:
            game_status = c.execute('''SELECT game_status FROM games WHERE rowid = ?;''', (game_id,)).fetchone()[0]
        except Exception:  # invalid game id
            return "0"

        # if game hasn't started yet
        if game_status == "start":
            c.execute('''UPDATE games SET game_status = ? WHERE rowid = ?;''', ('in_game', game_id))

            # Successful!
            return "1"
        else:
            return "2"
    

def garbage_collect_games():
    """
    Check for games that are still in the database after 24 hours
    """

    delta_24_hours = datetime.timedelta(days=1)

    with sqlite3.connect(moosic_db) as c:

        #check for all games that haven't been monitored for 24 hrs
        disconnected_games = c.execute('''SELECT rowid FROM games WHERE disconnect_check < ?;''', (datetime.datetime.now() - delta_24_hours,)).fetchall()

        #garbage collect each disconnected game
        for game in disconnected_games:
            game_id = game[0]
            garbage_collect(c, game_id)