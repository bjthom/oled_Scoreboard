import urllib2
from httplib import BadStatusLine
import time
import datetime
import xml.etree.ElementTree as ET
import serial
import sys

# Usage: python oled_mlb.py <port> <gid>

# Grab input
port = sys.argv[1]
gid = sys.argv[2]

# Initialize exception counters
html_excepts = 0
parse_excepts = 0

# Build URLs from GID
# - http://gd2.mlb.com/components/game/mlb/<data><gid>/plays.xml
# - http://gd2.mlb.com/components/game/mlb/<data><gid>/gamecenter.xml
# - http://gd2.mlb.com/components/game/mlb/<data><gid>/game_events.xml

split_id = gid.split('_')
year = 'year_' + split_id[1] + '/'
month = 'month_' + split_id[2] + '/'
day = 'day_' + split_id[3] + '/'
url_setup = 'http://gd2.mlb.com/components/game/mlb/' + year + month + day + gid
url_plays = url_setup + '/plays.xml'
url_gamecenter = url_setup + '/gamecenter.xml'
url_event = url_setup + '/game_events.xml'

# Check game time if not yet started,
# Exit nicely and say why if something goes wrong
# << Not yet implemented >>
try:
    game_info = urllib2.urlopen(url_gamecenter)
    info_tree = ET.parse(game_info)
    info = info_tree.getroot()
    #game_time = info.attrib['game_time_et']

except urllib2.HTTPError, e:
    print 'code: ', e.code
    sys.exit()
    
# Each team should be associated with a 2 character digit, substitute space if i<10
team_dict = {'ana':' 0', 'ari':' 1', 'atl':' 2', 'bal':' 3', 'bos':' 4', 'cha':' 5', \
             'chn':' 6', 'cin':' 7', 'cle':' 8', 'col':' 9', 'det':'10', 'mia':'11', \
             'hou':'12', 'kca':'13', 'lan':'14', 'mil':'15', 'min':'16', 'nya':'17', \
             'nyn':'18', 'oak':'19', 'phi':'20', 'pit':'21', 'sdn':'22', 'sea':'23', \
             'sfn':'24', 'sln':'25', 'tba':'26', 'tex':'27', 'tor':'28', 'was':'29'}

# Get teams from GID
away_team = split_id[4][:3]
home_team = split_id[5][:3]
teams = team_dict[away_team] + team_dict[home_team]
    
# Open serial connection to board
ser = serial.Serial(port, 9600)

while 1:
    try:
        # Open URL and parse XML tree
        data = urllib2.urlopen(url_plays)
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
        # inning state is the first letter of
        #  the inning status, unless game
        #  is over or not yet started
        inning = game.attrib['inning']
        inn_state = game.attrib['inning_state']
        status = game.attrib['status_ind']

        if len(inning) == 1:
            inning = inning.ljust(2)

        if status == 'I':
            inn = inn_state[0].lower()
        elif (status == 'P') or (status == 'F') or (status == 'O'):
            inn = status.lower()
        elif status == 'PR':
            inn = 'p' #change to rain?
        elif status == 'PW':
            inn = 'w'
        else:
            inn = '0'

        # Get baserunner info from:
        #   game->field->offense->man['bnum']
        # Get the base number(s) occupied,
        #   convert to int, and then reduce and OR to get onbase code
        offense = game_kids[4].getchildren()[0]
        runners = offense.getchildren()

        bases = [i.attrib['bnum'] for i in runners]
        bases = [int(i) for i in bases if i != '4']
        
        if len(bases) > 0:
            onbase = reduce(lambda x,y: x|y, bases)
        else:
            onbase = 0;
            
        # Get at bat info
        # - Names should be no longer than 14 characters
        #   Longest name: len(Saltalamacchia) == 14
        
        players = game_kids[1].getchildren()

        batter = players[0].attrib['boxname']
        pitcher = players[1].attrib['boxname']

        batter = batter.ljust(14)
        pitcher = pitcher.ljust(14)

        # Get at bat event info
        at_bat = game_kids[3].getchildren()

        if len(at_bat) == 0:
            last_play = 'None'
        else:
            last_play = at_bat[-1].attrib['des']

        if len(last_play) < 20:
            last_play = last_play.ljust(20)
        if len(last_play) > 20:
            last_play = last_play[:20]
            
        # Set up serial data
        gameinfo = inn+inning+teams+rhe+count+str(onbase)
        ser_data = gameinfo
        inn_len = len(inn) + len(inning)
        data_len = len(ser_data) + len(batter) + len(pitcher)

        # Print information
        print "Inn      (%2d): \"%s%s\"" % (inn_len, inn, inning)
        print "Teams    (%2d): \"%s vs. %s (%s)\"" % (len(teams), away_team, home_team, teams)
        print "RHE      (%2d): \"%s\"" % (len(rhe), rhe)
        print "Count    (%2d): \"%s\"" % (len(count), count)
        print "Runners       : \"%d (%s)\"" % (onbase, ','.join([`i` for i in bases]))
        print "Batter   (%2d): \"%s\"" % (len(batter), batter)
        print "Pitcher  (%2d): \"%s\"" % (len(pitcher), pitcher)
        print "Last Play(%2d): \"%s\"" % (len(last_play), last_play)
        print "Sending  (%2d): \"#%s%s%s*\"" % (data_len, ser_data, batter, pitcher)
        print "Exceptions    : %d, %d" % (html_excepts, parse_excepts)
        print 20*'='

        # Write data to serial port
        ser.write('#')
        ser.write(ser_data)
        ser.write(batter)
        ser.write(pitcher)
        ser.write('*')
        #print ser.read(55)
                
        # Wait before repolling website 
        # 12 sec. "max" between pitch time
        time.sleep(15)
    
    except (urllib2.HTTPError, urllib2.URLError, httplib.BadStatusLine):
        num_excepts += 1
        pass

    except ET.ParseError:
        parse_excepts +=1
        pass

    except KeyboardInterrupt:
        # Stop script cleanly after keyboard interrupt
        ser.close()
        print '\nScoreboard Done.'
        break
