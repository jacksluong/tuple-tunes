import numpy
import sqlite3
import datetime

moosic_db = '/var/jail/home/team59/moosic.db'


def fetch(game_id, username, last_updated_measure):
    '''given the game id, username, and last updated measure of fetching player,
    checks if game state needs to be updated,
    if so, returns the new game state along with the player who's turn it is'''

    with sqlite3.connect(moosic_db) as c:
        #update the player last ping for timeout purposes
        try:
            c.execute('''UPDATE players SET last_ping = ? WHERE game_id = ? AND username = ?;''', (datetime.datetime.now(), game_id, username))
        except Exception as e:
            return "INVALID GAME ID!"

        #check for status of the game
        game_status = c.execute('''SELECT game_status FROM games WHERE rowid = ?;''', (game_id, )).fetchone()

        if game_status[0] == 'ended':
            return f"Your game with game_id {game_id} has ended"
        
        #if game is started status, fetch returns all current players
        if game_status[0] == 'start':
            players = c.execute('''SELECT username FROM players WHERE game_id = ?;''',(game_id,)).fetchall()
            num_players = len(players)

            #get player names
            player_names = [val[0] for val in players]

            return f"Game has not started \n Num Players: {num_players} \n Current Players: {', '.join(player_names)}"

        #if currently in game: 
        if game_status[0] == 'in_game':

            #check if we need to update the number of measures
            current_measure, turn = c.execute('''SELECT measure, turn FROM games WHERE rowid = ?;''', (game_id, )).fetchone()

            if last_updated_measure == current_measure:
                return f"Your info is all up to date :) currently game is at measure {last_updated_measure}"
            
            #if we need to update things, we return the whole entire song again
            song_info = c.execute('''SELECT * FROM measures WHERE game_id = ? ORDER BY measure_number ASC;''', (game_id,)).fetchall()
            song = ''
            #return song_info
            for measure in song_info:
                song = song + ' '.join(measure[3:19]) + '\n'
            
            #c.execute('''SELECT n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,16)

            #figure out who's turn it should be
            players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()
            #return f"Players {players} turn {turn} song {song}"
            in_turn = players[turn % len(players)][0] #username of player in turn

            return f"Player in turn: {in_turn} \n Current Song: \n {song}"

        
        return f"No matching game status found for {game_status[0]}"

def play_turn(game_id, username, measure):
    '''given game id, username, and new measure,
    updates the turn in the game, and returns that measure was successfully added'''

    with sqlite3.connect(moosic_db) as c:
        #first, update ping
        try:
            c.execute('''UPDATE players SET last_ping = ? WHERE game_id = ? AND username = ?;''', (datetime.datetime.now(), game_id, username))
        except Exception as e:
            return "INVALID GAME ID OR USERNAME"

        last_measure, turn = c.execute('''SELECT measure, turn FROM games WHERE rowid = ?;''',(game_id,)).fetchone()
        
        c.execute('''CREATE TABLE IF NOT EXISTS measures(game_id real, username text, measure_number int, n1 text, n2 text, n3 text, n4 text, n5 text, n6 text, n7 text, n8 text, n9 text, n10 text, n11 text, n12 text, n13 text, n14 text, n15 text, n16 text);''')
        c.execute('''INSERT INTO measures VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);''',(game_id,username, last_measure+1, measure[0],measure[1],measure[2],measure[3],measure[4],measure[5],measure[6],measure[7],measure[8],measure[9],measure[10],measure[11],measure[12],measure[13],measure[14],measure[15]))

        new_measure = last_measure + 1
        turn = turn + 1
        c.execute('''UPDATE games SET turn = ?, measure = ? WHERE rowid = ?;''', (turn, new_measure, game_id))

        return f"New measure {new_measure} inserted into game_id {game_id} by {username}: \n {measure}"


def update_last_ping(game_id, username):
    '''given game id and username,
    updates the last ping of the player'''
    with sqlite3.connect(moosic_db) as c:
        #first, update ping
        try:
            c.execute('''UPDATE players SET last_ping = ? WHERE game_id = ? AND username = ?;''', (datetime.datetime.now(), game_id, username))
        except Exception as e:
            return "INVALID GAME ID OR USERNAME"

        return f"Last ping of player {username} updated"

def end_game(game_id):
    with sqlite3.connect(moosic_db) as c:
        c.execute('''UPDATE games SET game_status = ? WHERE rowid = ?;''', ("ended", game_id))
        
        song_info = c.execute('''SELECT * FROM measures WHERE game_id = ? ORDER BY measure_number ASC;''', (game_id,)).fetchall()
        song = ''
        for measure in song_info:
            song = song + ' '.join(measure[3:19]) + '\n'

        return f"Game {game_id} has ended \n Final song: \n {song}"



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

