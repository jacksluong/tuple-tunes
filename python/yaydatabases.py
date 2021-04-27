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

def initiate_tables():
    with sqlite3.connect(moosic_db) as c:
        c.execute('''CREATE TABLE IF NOT EXISTS games(game_code real, host_id text, key text, tempo text, game_status text, time timestamp);''')
        #game code - a number btwn 1 - 99 that increments
        #game status - 'start', 'in-game', 'ended'
        #key - one of the 12 major scales
        #tempo - 

        c.execute('''CREATE TABLE IF NOT EXISTS players(game_id real, username text, measure real);''')
        c.execute('''CREATE TABLE IF NOT EXISTS measures(game_id real, username text, measure_number real, n1 text, n2 text, n3 text, n4 text, n5 text, n6 text, n7 text, n8 text, n9 text, n10 text, n11 text, n12 text, n13 text, n14 text, n15 text, n16 text,);''')

def create_game(host_id, key, tempo):
    '''creates a new game row
    returns the game code'''
    game_code = 1 #default

    with sqlite3.connect(moosic_db) as c:
        c.execute('''CREATE TABLE IF NOT EXISTS games(game_code real, host_id text, key text, tempo text, game_status text, time timestamp);''')
        #the new game code is going to be the most recent one + 1
        most_recent = c.execute('''SELECT game_code FROM games ORDER BY timing ASC;''').fetchone()
        if most_recent:
            game_code = most_recent[0] + 1 % 100 + 1

        c.execute('''INSERT into games VALUES (?,?,?,?,?,?);''', (game_code, host_id, key, tempo, 'start', datetime.datetime.now()))
    
    return game_code



def request_handler(request):
    if request["method"] == "POST":
        return "this is a post request"

    elif request["method"] == "GET":
        return "this is a get request"

    else:
        return 'idk what method you did lol'