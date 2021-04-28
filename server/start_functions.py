import numpy
import sqlite3
import datetime

moosic_db = '/var/jail/home/team59/moosic/moosic.db'

#hard coded stuff: all the major minor scales
#TODO: set up frequencies with each note

key_scale_notes = {
    "C Major": ["C", "D", "E", "F", "G", "A", "B", "C"],
    "G Major": ["G", "A", "B", "C", "D", "E", "F#", "G"],
    "D Major": ["D", "E", "F#", "G", "A", "B", "C#", "D"],
    "A Major": ["A", "B", "C#", "D", "E", "F#", "G#", "A"],
    "E Major": ["E", "F#", "G#", "A", "B", "C#", "D#", "E"],
    "B Major": ["B", "C#", "D#", "E", "F#", "G#", "A#", "B"],
    "F# Major": ["F#", "G#", "A#", "B", "C#", "D#", "F", "F"],
    "Db Major": ["Db", "Eb", "F", "Gb", "Ab", "Bb", "C", "Db"],
    "Eb Major": ["Eb", "F", "G", "Ab", "Bb", "C", "D", "Eb"],
    "Bb Major": ["Bb", "C", "D", "Eb", "F", "G", "A", "Bb"],
    "F Major": ["F", "G", "A", "Bb", "C", "D", "E", "F"],
}

GAME_ROOM_CAPACITY_LIMIT = 10

def initiate_tables():
    """
    Tables to be created in database
    """
    with sqlite3.connect(moosic_db) as c:
        c.execute('''CREATE TABLE IF NOT EXISTS games(game_code int, host text, key text, tempo text, game_status text, time timestamp, turn int, measure int);''')
        #game code - a number btwn 1 - 99 that increments
        #game status - 'start', 'in-game', 'ended'
        #key - one of the 12 major scales
        #tempo - 

        c.execute('''CREATE TABLE IF NOT EXISTS players(game_id real, username text, last_ping timestamp, entry_time timestamp);''')

        c.execute('''CREATE TABLE IF NOT EXISTS measures(game_id real, username text, measure_number int, n1 text, n2 text, n3 text, n4 text, n5 text, n6 text, n7 text, n8 text, n9 text, n10 text, n11 text, n12 text, n13 text, n14 text, n15 text, n16 text,);''')

def create_game(host, key, tempo):
    '''creates a new game row
    returns the game code'''
    game_code = 0 #default

    with sqlite3.connect(moosic_db) as c:

        #create GAMES TABLE
        c.execute('''CREATE TABLE IF NOT EXISTS games(game_code int, host text, key text, tempo text, game_status text, turn int, measure int, time timestamp);''')
        
        #CREATE PLAYERS TABLE
        c.execute('''CREATE TABLE IF NOT EXISTS players(game_id real, username text, last_ping timestamp, entry_time timestamp);''')
        
        #create a row in game table
        #the new game code is going to be the most recent one + 1
        most_recent = c.execute('''SELECT game_code FROM games ORDER BY time DESC;''').fetchone()
        if most_recent:
            game_code = (most_recent[0] + 1) % 100

        c.execute('''INSERT INTO games VALUES (?,?,?,?,?,?,?,?);''', (game_code, host, key, tempo, 'start', 0, 0, datetime.datetime.now()))

        #get game_id of last inserted game
        game_id = c.execute('''SELECT rowid FROM games ORDER BY time DESC;''').fetchone()
        game_id = game_id[0]

        #create a row in players
        c.execute('''INSERT INTO players VALUES (?,?,?,?);''', (game_id, host, datetime.datetime.now(), datetime.datetime.now()))

    #turn game code into 3 length string
    game_code_string = '0' * (3 - len(str(game_code))) + str(game_code)

    #TODO: Reformat return later on: (currently for demo purposes)
    return f"Game Created Successfully! Game Code: {game_code_string}. Game ID: {game_id}. Key: {key}. Tempo: {tempo}. Host: {host}"

def join_game(username, game_code):
    """
    Allow player with given username to join a game with the given game_code
    """

    with sqlite3.connect(moosic_db) as c:

        #get game_id and game_status of game 
        game = c.execute('''SELECT rowid, game_status FROM games WHERE game_code = ?;''',(int(game_code),)).fetchone()

        if not game:
            return "No Game Room Found"

        game_id, game_status = game

        #check if game is in start state
        if game_status == "start":
            #check num of players
            players = c.execute('''SELECT * FROM players WHERE game_id = ?;''',(game_id,)).fetchall()
            num_players = len(players)
            
            #check if room is still open to players
            if num_players < GAME_ROOM_CAPACITY_LIMIT:

                #insert player into database
                c.execute('''INSERT INTO players VALUES (?,?,?,?);''',(game_id, username, datetime.datetime.now(), datetime.datetime.now()))

                #TODO: Reformat return later on: (currently for demo purposes)
                return f"{username} has successfully joined game {game_code} with {num_players + 1} players. The game_id is {game_id}"

            else:
                return "Failed to Join Game: Reached Capacity Limit"

        else:
            return "Game Has Already Started/Ended. Please provide different game code"

def start_game(game_id):
    """
    Start the game with the give game id
    """
    with sqlite3.connect(moosic_db) as c:
        c.execute('''UPDATE games SET game_status = ? WHERE game_id = ?;''', ('in_game', game_id))
        return f"Game w/ Game ID: {game_id} has started"