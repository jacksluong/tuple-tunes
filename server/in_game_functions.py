import numpy
import sqlite3
import datetime 

moosic_db = '/var/jail/home/team59/moosic6.db'
MAX_MEASURES = 11

def fetch(game_id, username, last_updated_measure):
    '''given the game id, username, and last updated measure of fetching player,
    checks if game state needs to be updated,
    if so, returns the new game state along with the player who's turn it is'''

    with sqlite3.connect(moosic_db) as c:
        #update the player last ping for timeout purposes
        ping_status = update_last_ping(game_id, username)
        if ping_status == "0":
            #invalid game ID!
            return ping_status

        #check for status of the game
        # game_code int, host text, key text, tempo text, game_status text, time timestamp, turn int, measure int
        game_status, current_measure, turn = c.execute('''SELECT game_status, measure, turn FROM games WHERE rowid = ?;''', (game_id,)).fetchone()

        if game_status == "ended":
            return 'game ended'

        # get song from player's last measure and onwards
        song = get_song(game_id, last_updated_measure)

        # figure out who's turn it should be
        players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()

        # username of player in turn
        in_turn = players[turn % len(players)][0]

        ###FOR PRINT DEBUGGING AND SHOW ON POSTMAN
        players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()

        player_names = [player[0] for player in players]

        in_turn = c.execute('''SELECT turn FROM games WHERE rowid = ?;''', (game_id,)).fetchone()[0] % len(player_names)

        # return f'players remaining: {player_names}, in turn index: {in_turn}'


        return f"{in_turn}&{current_measure}&{song} "



def play_turn(game_id, username, measure):
    '''given game id, username, and new measure,
    updates the turn in the game, and returns that measure was successfully added'''

    with sqlite3.connect(moosic_db) as c:
        #first, update ping
        ping_status = update_last_ping(game_id, username)
        if ping_status == "0":
            return ping_status

        measure_num, turn = c.execute('''SELECT measure, turn FROM games WHERE rowid = ?;''',(game_id,)).fetchone()
        # measure is the (0-indexed) index of the measure that we are submitting rn)

        if measure_num >= MAX_MEASURES:
            return '0'
        
        c.execute('''CREATE TABLE IF NOT EXISTS measures(game_id int, username text, measure_number int, n1 int, n2 int, n3 int, n4 int, n5 int, n6 int, n7 int, n8 int, n9 int, n10 int, n11 int, n12 int, n13 int, n14 int, n15 int, n16 int);''')

        c.execute('''INSERT INTO measures VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);''',(game_id,username, measure_num, measure[0],measure[1],measure[2],measure[3],measure[4],measure[5],measure[6],measure[7],measure[8],measure[9],measure[10],measure[11],measure[12],measure[13],measure[14],measure[15]))

        c.execute('''UPDATE games SET turn = ?, measure = ? WHERE rowid = ?;''', (turn + 1, measure_num + 1, game_id))

        #once we hit the limit of measures, END THE GAME
        if measure_num + 1 == MAX_MEASURES:
            c.execute('''UPDATE games SET game_status = ? WHERE rowid = ?;''', ("ended", game_id))

        #SUCCESSFUL!
        return "1"

def leave_game(c, game_id, username):
    """
    Allow a user to leave a game, update game state accordinally
    """

    #get the current turn in the game (not modded yet)
    try:
        turn = c.execute('''SELECT turn FROM games WHERE rowid = ?;''', (game_id,)).fetchone()[0]
    except Exception as e:
        return e

    #get player_names
    players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()
    player_names = [player[0] for player in players]

    #find the index of the player to be popped
    pop_index = player_names.index(username)

    #index of current turn
    turn_index = turn % len(players)

    #only update turn if pop_index is less than turn_index
    if (pop_index < turn_index) or (pop_index == turn_index and pop_index == (len(players) - 1)):
        c.execute('''UPDATE games SET turn = ? WHERE rowid = ?;''', (turn_index - 1, game_id))

    #delete player from players database
    c.execute('''DELETE FROM players WHERE username = ?;''', (username, ))

    #if tehre was only one player and they left, game ends
    if len(player_names) == 1:
        c.execute('''DELETE FROM games WHERE rowid = ?; ''', (game_id,))
        return 'game_ended'

    #FOR DEBUGGIN PURPOSES
    players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()

    player_names = [player[0] for player in players]

    in_turn = c.execute('''SELECT turn FROM games WHERE rowid = ?;''', (game_id,)).fetchone()[0] % len(player_names)

    # return f'YEETED away {username} \n players remaining: {player_names}, in turn index: {in_turn}'

    #SUCCESSFUL!
    return "1"


def monitor_disconnect(c, game_id, time_now):
    """
    makes sure no players are disconnected/idle, update game state 
    """
    
    #create time delta variables
    delta_time_20 = datetime.timedelta(seconds=20)
    delta_time_30 = datetime.timedelta(seconds=30)

        
    #check last time we checked for disconnect
    last_disconnect_check = c.execute('''SELECT disconnect_check FROM games WHERE rowid = ?;''',(game_id,)).fetchone()[0]

    dto = datetime.datetime.strptime(last_disconnect_check,'%Y-%m-%d %H:%M:%S.%f')

    # return f'last disconnect check: {last_disconnect_check} ({type(last_disconnect_check)}), dto: {dto} ({type(dto)})'

    #only check if we haven't checked for 30 seconds
    if time_now - dto > delta_time_30:


        #get all the players who haven't pinged/fetched for the last 20 seconds
        players_disconnect = c.execute('''SELECT username FROM players WHERE game_id = ? AND last_ping < ?;''',(game_id, time_now - delta_time_20)).fetchall()

        disconnected_names = [player[0] for player in players_disconnect]


        # return f"before for loop: {disconnected_names}"
            
        #for each player that disconnect, remove them from the game
        for username in disconnected_names:
            leave_game(c, game_id, username) 

        #update last disconnect check time to be current time
        c.execute('''UPDATE games SET disconnect_check = ? WHERE rowid = ?;''',(datetime.datetime.now(), game_id))

        return f"Checked everyone else: time_now: {time_now}, dto: {dto}, yeeted away: {disconnected_names}"


    return "no need to check: time_now: {time_now}, dto: {dto}"
        

def update_last_ping(game_id, username):
    '''given game id and username,
    updates the last ping of the player'''
    with sqlite3.connect(moosic_db) as c:
        #first, update ping
        try:
            c.execute('''UPDATE players SET last_ping = ? WHERE game_id = ? AND username = ?;''', (datetime.datetime.now(), game_id, username))
            
            monitor_disconnect(c, game_id, datetime.datetime.now())

        except Exception as e:
            # return "INVALID GAME ID OR USERNAME"
            return f"0&{e}"

        #Successful!
        return "1"


def get_song(game_id, start_measure=0):
    """
    Given a game id, return the song created so far
    """
    with sqlite3.connect(moosic_db) as c:
        song_info = c.execute('''SELECT * FROM measures WHERE game_id = ? AND measure_number >= ? 
        ORDER BY measure_number ASC;''', (game_id, start_measure)).fetchall()

        #create song string
        song_list = []
        for measure in song_info:
            song_list.append(','.join(str(int_val) for int_val in measure[3:]))
        song = " ".join(song_list)

        return song
