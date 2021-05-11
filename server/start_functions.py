import sqlite3
import datetime

moosic_db = '/var/jail/home/team59/moosic1.db'

ROOM_CAPACITY = 10


def initiate_tables():
    """
    Tables to be created in database
    """
    with sqlite3.connect(moosic_db) as c:
        c.execute(
            '''CREATE TABLE IF NOT EXISTS games(game_code int, host text, key text, tempo text, game_status text, time timestamp, turn int, measure int);''')
        # game code - a number btwn 1 - 99 that increments
        # game status - 'start', 'in-game', 'ended'
        # key - one of the 12 major scales
        # tempo -

        c.execute(
            '''CREATE TABLE IF NOT EXISTS players(game_id real, username text, last_ping timestamp, entry_time timestamp);''')

        c.execute(
            '''CREATE TABLE IF NOT EXISTS measures(game_id real, username text, measure_number int, n1 text, n2 text, n3 text, n4 text, n5 text, n6 text, n7 text, n8 text, n9 text, n10 text, n11 text, n12 text, n13 text, n14 text, n15 text, n16 text,);''')


def create_game(host, key, tempo):
    '''creates a new game row
    returns the game code'''
    game_code = 0  # default

    with sqlite3.connect(moosic_db) as c:
        # create GAMES TABLE
        c.execute(
            '''CREATE TABLE IF NOT EXISTS games(game_code int, host text, key int, tempo int, game_status text, turn int, measure int, time timestamp);''')

        # CREATE PLAYERS TABLE
        c.execute(
            '''CREATE TABLE IF NOT EXISTS players(game_id real, username text, last_ping timestamp, entry_time timestamp);''')

        # create a row in game table
        # the new game code is going to be the most recent one + 1
        most_recent = c.execute('''SELECT game_code FROM games ORDER BY time DESC;''').fetchone()
        if most_recent:
            game_code = (most_recent[0] + 1) % 1000

        c.execute('''INSERT INTO games VALUES (?,?,?,?,?,?,?,?);''',
                  (game_code, host, key, tempo, 'start', 0, 0, datetime.datetime.now()))

        # get game_id of last inserted game
        game_id = c.execute('''SELECT rowid FROM games ORDER BY time DESC;''').fetchone()
        game_id = game_id[0]

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

            # get player names
            # player_names = [val[0] for val in players]

            # check if room is still open to players
            if num_players < ROOM_CAPACITY:

                # insert player into database
                c.execute('''INSERT INTO players VALUES (?,?,?,?);''',
                          (game_id, username, datetime.datetime.now(), datetime.datetime.now()))

                # successful!
                return f"3&{game_id}"

            else:
                # room capacity hit
                return "1"

        else:
            # game is in progress or ended
            return "2"


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
            return "2"

        elif status == "ended":
            return "3"
