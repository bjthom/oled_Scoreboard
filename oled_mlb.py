import urllib2
import time
import xml.etree.ElementTree as ET
import serial
import sys

# Usage: python oled_mlb.py <port> <gid>

# Grab input
port = sys.argv[1]
gid = sys.argv[2]

# Build URLs from GID
# - http://gd2.mlb.com/components/game/mlb/<data><gid>/plays.xml
# - http://gd2.mlb.com/components/game/mlb/<data><gid>/game.xml
# - http://gd2.mlb.com/components/game/mlb/<data><gid>/game_events.xml

split_id = gid.split('_')
year = 'year_' + split_id[1] + '/'
month = 'month_' + split_id[2] + '/'
day = 'day_' + split_id[3] + '/'
url_setup = 'http://gd2.mlb.com/components/game/mlb/' + year + month + day + gid
url = url_setup + '/plays.xml'
url_game = url_setup + '/game.xml'
url_event = url_setup + '/game_events.xml'

# Get game time if not yet started
game_info = urllib2.urlopen(url_game)
game_tree = ET.parse(game_info)
info = game_tree.getroot()
game_time = info.attrib['game_time_et']
    
# Each team should be associated with a 2 character digit, substitute space if i<10
team_dict = {'ana':' 0', 'ari':' 1', 'atl':' 2', 'bal':' 3', 'bos':' 4', 'cha':' 5', \
             'chn':' 6', 'cin':' 7', 'cle':' 8', 'col':' 9', 'det':'10', 'flo':'11', \
             'hou':'12', 'kca':'13', 'lan':'14', 'mil':'15', 'min':'16', 'nya':'17', \
             'nyn':'18', 'oak':'19', 'phi':'20', 'pit':'21', 'sdn':'22', 'sea':'23', \
             'sfn':'24', 'sln':'25', 'tba':'26', 'tex':'27', 'tor':'28', 'was':'29'}

# Get teams from GID
away_team = split_id[4][:3]
home_team = split_id[5][:3]
teams = team_dict[away_team] + team_dict[home_team]
    
##ser = serial.Serial(6, 9600)
while 1:
    try:
        # Open URL and parse XML tree
        data = urllib2.urlopen(url)
        tree = ET.parse(data)
            
        game = tree.getroot()
        game_kids = game.getchildren()

        # Get count data from
        #  game.attrib
        count = [game.attrib['b'],game.attrib['s'],game.attrib['o']]
        count = ''.join(count)

        # Get score and pad with spaces if necessary
        #  game->score.attrib
        score = game_kids[0].attrib

        arhe = [score['ar'],score['ah'],score['he']]
        hrhe = [score['hr'],score['hh'],score['ae']]

        arhe = [' '+x if len(x) == 1 else x for x in arhe]
        hrhe = [' '+x if len(x) == 1 else x for x in hrhe]

        rhe = ' '.join(arhe) + ' '.join(hrhe)
        
        # Get inning info
        #  game.attrib
        # pad inning number with space if < 10
        # 
        inning = game.attrib['inning']
        inn_state = game.attrib['inning_state']
        status = game.attrib['status_ind']

        if len(inning) == 1:
            inning = ' ' + inning

        if status == 'I':
            inn = inn_state[0].lower()
        elif status == 'P' or 'F' or 'O':
            inn = status.lower()
        elif status == 'PW':
            inn = 'w'
        else:
            inn = '0'

        # Get baserunner info from:
        #   game->field->offense->man['bnum']
        # Get the base number occupied,
        #   convert to int, and then OR to get onbase code
        offense = game_kids[4].getchildren()[0]
        runners = offense.getchildren()

        bases = [i.attrib['bnum'] for i in runners]
        bases = [int(i) for i in bases]
        
        onbase = any(bases)
            
        # Get at bat info
        at_bat = game_kids[3].getchildren()

        if len(at_bat) == 0:
            last_play = 'None'
        else:
            last_play = at_bat[-1].attrib['des']

        if len(last_play) < 20:
            addon = 20 - len(last_play)
            spaces = "                    "
            last_play = last_play + spaces[0:addon]
        if len(last_play) > 20:
            last_play = last_play[:20]
            
        # Set up serial data
        gameinfo = inn+inning+teams+rhe+count
        ser_data = gameinfo+last_play
        print 'Count: ' + count
        print 'RHE: ' + rhe
        print 'Inn: ' + inn + inning
        print onbase
        print 'Last Play: ' + last_play
        print len(last_play)
        print ser_data
        print '==='

        # Write data to serial port
        ##ser.write(ser_data)
                
        # Wait before repolling website 
        # 12 sec. "max" between pitch time
        time.sleep(15)
    except KeyboardInterrupt:
        #ser.close()
        print 'Scoreboard Done.'
        break

