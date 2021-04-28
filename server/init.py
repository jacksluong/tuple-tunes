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
        c.execute('''CREATE TABLE IF NOT EXISTS games(game_code int, host text, key text, tempo text, game_status text, turn int, measure int, time timestamp);''')
        c.execute('''CREATE TABLE IF NOT EXISTS players(game_id real, username text, last_ping timestamp, entry_time timestamp);''')

        
        #create a row in game table
        #the new game code is going to be the most recent one + 1
        most_recent = c.execute('''SELECT game_code FROM games ORDER BY time ASC;''').fetchone()
        if most_recent:
            game_code = (most_recent[0] + 1) % 100

        c.execute('''INSERT INTO games VALUES (?,?,?,?,?,?,?,?);''', (game_code, host, key, tempo, 'start', 0, 0, datetime.datetime.now()))
        #game_id = c.lastrowid
        game_id = c.execute('''SELECT rowid FROM games ORDER BY time ASC;''').fetchone()
        game_id = game_id[0]
        #create a row in players
        c.execute('''INSERT INTO players VALUES (?,?,?,?);''', (game_id, host, datetime.datetime.now(), datetime.datetime.now()))

    game_code_string = '0' * (3 - len(str(game_code))) + str(game_code)
    return (game_code_string, str(game_id))

def join_game(username, game_code):
    with sqlite3.connect(moosic_db) as c:
        game = c.execute('''SELECT rowid, status FROM games WHERE game_code = ?;''',(game_code))
        game_id, game_status = game

        #check if game is in start state
        if game_status == "start":
            #check num of players
            players = c.execute('''SELECT rowid FROM players WHERE game_id = ?;''',(game_id))
            num_players = len(players)
            
            if num_players < GAME_ROOM_CAPACITY_LIMIT:
                #insert player into database
                c.execute('''INSERT INTO players VALUES (?,?,?,?);'''(game_id, username, datetime.datetime.now(), datetime.datetime.now()))

                return f"{username} has successfully joined game {game_code} with {num_players + 1} players. The game_id is {game_id}"
            else:
                return "Failed to Join Game: Reached Capacity Limit"

        else:
            return "Game Has Already Started. Please provide different game code"

def request_handler(request):
    if request["method"] == "POST":
        try:
            type_ = request['form']['type']
            #types of post requests:
            #create - creates a new game
            #join - join a game
            #start - start a game
            #measure - adds a measure to the song
            #ping - updates the ping

        except Exception as e:
            return "Error: please return a type - create, join, start, measure, ping"
        
        print("you have made a " + type_ + " request")

        ###############################
        # CREATE A GAME ###############
        ###############################

        if type_ == "create":
            try:
                host = request['form']['host']
                key = request['form']['key']
                tempo = request['form']['tempo']
            except Exception as e:
                return "Please return host, key, tempo when creating a game"

            print("Game was created with host, key, tempo ", host, key, tempo)

            game_code, game_id = create_game(host, key, tempo)

            return "Your game code is: " + game_code + ", game id: " + game_id

        ###############################
        # JOIN A GAME ###############
        ###############################
        elif type_ == "join":
            try:
                username = request['form']['username']
                game_code = request['form']['game_code']
            except Exception as e:
                return "Please provide username and game code when joining a game"
            
            return join_game(username, game_code)

        #TODO: Have to live update players currently waiting with number of players in the current room (currently is static update, only when join)
        #TODO: Maybe insert num_players into game_state?

        return "this is a post request"

    elif request["method"] == "GET":
        return "this is a get request"

    else:
        return 'idk what method you did lol'