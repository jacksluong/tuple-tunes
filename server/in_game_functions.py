import numpy
import sqlite3
import datetime

moosic_db = '/var/jail/home/team59/moosic1.db'


def fetch(game_id, username, last_updated_measure):
    '''given the game id, username, and last updated measure of fetching player,
    checks if game state needs to be updated,
    if so, returns the new game state along with the player who's turn it is'''

    with sqlite3.connect(moosic_db) as c:
        #update the player last ping for timeout purposes
        update_last_ping(game_id, username)

        #check for status of the game
        game_status = c.execute('''SELECT game_status FROM games WHERE rowid = ?;''', (game_id,)).fetchone()

        #game has ended
        if game_status[0] == 'ended':
            return '1'

        #if currently in game: 
        elif game_status[0] == 'in_game':

            #check if we need to update the number of measures
            current_measure, turn = c.execute('''SELECT measure, turn FROM games WHERE rowid = ?;''', (game_id, )).fetchone()

            if last_updated_measure == current_measure:

                #up to date!
                return "3"
            
            #get song created so far
            song = get_song(game_id)

            #figure out who's turn it should be
            players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()

            #username of player in turn
            in_turn = players[turn % len(players)][0]

            # return f"Player in turn: {in_turn} \n Current Song: \n {song}"
            return f"4&{in_turn}&{current_measure}&{song}"



def play_turn(game_id, username, measure):
    '''given game id, username, and new measure,
    updates the turn in the game, and returns that measure was successfully added'''

    with sqlite3.connect(moosic_db) as c:
        #first, update ping
        update_last_ping(game_id, username)

        last_measure, turn = c.execute('''SELECT measure, turn FROM games WHERE rowid = ?;''',(game_id,)).fetchone()
        
        c.execute('''CREATE TABLE IF NOT EXISTS measures(game_id int, username text, measure_number int, n1 int, n2 int, n3 int, n4 int, n5 int, n6 int, n7 int, n8 int, n9 int, n10 int, n11 int, n12 int, n13 int, n14 int, n15 int, n16 int);''')

        c.execute('''INSERT INTO measures VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);''',(game_id,username, last_measure+1, measure[0],measure[1],measure[2],measure[3],measure[4],measure[5],measure[6],measure[7],measure[8],measure[9],measure[10],measure[11],measure[12],measure[13],measure[14],measure[15]))

        new_measure = last_measure + 1
        turn = turn + 1
        c.execute('''UPDATE games SET turn = ?, measure = ? WHERE rowid = ?;''', (turn, new_measure, game_id))

        # return f"New measure {new_measure} inserted into game_id {game_id} by {username}: \n {measure}"
        return f"1&{new_measure}"


def update_last_ping(game_id, username):
    '''given game id and username,
    updates the last ping of the player'''
    with sqlite3.connect(moosic_db) as c:
        #first, update ping
        try:
            c.execute('''UPDATE players SET last_ping = ? WHERE game_id = ? AND username = ?;''', (datetime.datetime.now(), game_id, username))
        except Exception as e:
            # return "INVALID GAME ID OR USERNAME"
            return "0"

        # return f"Last ping of player {username} updated"
        return "1"

def end_game(game_id):
    with sqlite3.connect(moosic_db) as c:
        try:    
            game_status = c.execute('''SELECT game_status FROM games WHERE rowid = ?;''', (game_id,)).fetchone()[0]
        except:
            return "0"

        if game_status == "ended":
            return "1"
        
        c.execute('''UPDATE games SET game_status = ? WHERE rowid = ?;''', ("ended", game_id))
        
        #get song info
        song = get_song(game_id)

        #sucessful! return final song
        return "2&{song}"

def get_song(game_id):
    """
    Given a game id, return the song created so far
    """
    with sqlite3.connect(moosic_db) as c:
        song_info = c.execute('''SELECT * FROM measures WHERE game_id = ? ORDER BY measure_number ASC;''', (game_id,)).fetchall()

        #create song string
        song_list = []
        for measure in song_info:
            song_list.append(','.join(str(int_val) for int_val in measure[3:19]))
        song = " ".join(song_list)

        return song

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

