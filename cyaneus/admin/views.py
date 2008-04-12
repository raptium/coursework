from django.shortcuts import render_to_response
from django.http import HttpResponse
from django.http import Http404
from django.core import serializers
from datetime import datetime
from google.appengine.ext import db
from google.appengine.api import users
from live.models import *
import logging
import urllib

def console(request):
	if not users.is_current_user_admin():
		raise Http404
	return render_to_response('admin/admin.html', {})
	
	
def addEntry(request):
	ostr = ''
	if request['channel']:
		msg = 'Channel already exists.<br />'	
		schs = request['channel'].decode('utf-8').rsplit()
		for sch in schs:
			query = Channel.all().filter('name =', sch)
			if query.count() == 0:
				channel = Channel(name=sch)
				channel.put()
				msg = "Channel added.<br />"
		ostr += msg
	if request['game']:
		if Game.all().filter('name =', request['game'].decode('utf-8')).count() == 0:
			game = Game(name=request['game'].decode('utf-8'))
			game.put()
			ostr += "Game added.<br />"
		else:
			ostr += "Game already exists.<br />"
	if request['team']:
		msg = 'Team already exists.<br />'
		steams = request['team'].decode('utf-8').rsplit()
		for steam in steams:
			query = Team.all().filter('name =', steam)
			if query.count() == 0:
				team = Team(name=steam)
				team.put()
				msg = "Team added.<br />"
		ostr += msg
	if request['tool']:
		if Tool.all().filter('name =', request['tool'].decode('utf-8')).count() == 0:
			tool = Tool(name=request['tool'].decode('utf-8'))
			tool.put()
			ostr += "Tool added.<br />"
		else:
			ostr += "Tool already exists.<br />"
	return HttpResponse('<p>' + ostr + '</p>')
	
	
def addMethod(request):
	ostr = 'Error.'
	if request['channel'] and request['tool'] and request['reference']:
		query = Channel.all().filter('name =', request['channel'].decode('utf-8'))
		if query.count() == 0:
			return HttpResponse('<p>Method Error.</p>')
		channel = query[0]
		query = Tool.all().filter('name =', request['tool'].decode('utf-8'))
		if query.count() == 0:
			return HttpResponse('<p>Method Error.</p>')
		tool = query[0]
		if Method.all().filter('channel =', channel).filter('tool =', tool).filter('reference =', request['reference'].decode('utf-8')).count() > 0:
			return HttpResponse('<p>Method already exists.</p>')
		method = Method(channel=channel, tool=tool, reference=request['reference'].decode('utf-8'))
		method.put()
		ostr = 'Method added.'
	return HttpResponse('<p>' + ostr + '</p>')
	
def addMatch(request):
	msg = ''
	if not request['time']:
		return HttpResponse('<p>No begin time.</p>')
	try:
		begin_time = datetime.strptime(request['time'], '%Y %m %d %H:%M')
	except:
		return HttpResponse('<p>Can not parse time.</p>')
	
	if request['channel'] and request['game'] and request['team']:
		query = Game.all().filter('name =', request['game'].decode('utf-8'))
		if query.count() == 0:
			return HttpResponse('<p>No Such Game.</p>')
		game = query[0]
		steams = request['team'].decode('utf-8').rsplit()
		teams = []
		for steam in steams:
			query = Team.all().filter('name =', steam)
			if query.count() == 0:
				return HttpResponse('<p>No Such Game.</p>')
			teams.append(query[0].key())
		schs = request['channel'].decode('utf-8').rsplit()
		channels = []
		for sch in schs:
			query = Channel.all().filter('name =', sch)
			if query.count() == 0:
				return HttpResponse('<p>No Such Channel.</p>')
			channels.append(query[0].key())
		match = Match()
		match.game = game
		match.channels = channels
		match.teams = teams
		match.begin_time = begin_time
		match.put()
		msg = 'Match added'
	return HttpResponse('<p>' + msg + '</p>')
		
		
