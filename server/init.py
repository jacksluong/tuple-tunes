import sys
sys.path.append('__HOME__/server')
from start_functions import *

def request_handler(request):

    #POST REQUEST HANDLERS
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

        # host creates game
        if type_ == "create":
            # return request
            try:
                host = request['form']['username']
                key = request['form']['key']
                tempo = request['form']['tempo']
            except Exception as e:
                return "Please return host username, key, tempo when creating a game"

            return create_game(host, key, tempo)


        #players join game
        elif type_ == "join":

            #get values from request
            try:
                username = request['form']['username']
                game_code = request['form']['game_code']
            except Exception as e:

                #not enough data provided
                return "Please provide username and game code when joining a game"
            
            return join_game(username, game_code)

        #start a game
        elif type_ == "start":
            
            #get values from request
            try:
                game_id = int(request['form']['game_id'])
            except:
                return "Please provide a valid game id"

            return start_game(game_id)

        #TODO: Have to live update players currently waiting with number of players in the current room (currently is static update, only when join)
        #TODO: Maybe insert num_players into game_state?

        else:
            return "Invalid POST request! Please specify valid type"

    #GET REQUEST HANDLERS
    elif request["method"] == "GET":
        try:
            game_id = int(request['values']['game_id'])
        except:
            return "Please provide valid game id!"

        return fetch_game_status(game_id)

    #INVALID REQUEST
    else:
        return 'INVALID REQUEST TYPE: ONLY POST OR GET REQUESTS'