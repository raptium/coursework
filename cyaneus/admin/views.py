from django.shortcuts import render_to_response
from django.http import HttpResponseRedirect
from django.http import HttpResponse
from google.appengine.ext import db
from google.appengine.api import users
from live.models import *
import logging
import urllib

def console(request):
	if not users.is_current_user_admin():
		return HttpResponseRedirect('../')
	return render_to_response('admin/admin.html', {})
	
	
def addEntry(request):
	ostr = ''
	if request['channel']:
		if Channel.all().filter('name =', request['channel'].decode('utf-8')).count() == 0:
			channel = Channel(name=request['channel'].decode('utf-8'))
			channel.put()
			ostr += "Channel added.<br />"
		else:
			ostr += "Channel already exists.<br />"
	if request['game']:
		if Game.all().filter('name =', request['game'].decode('utf-8')).count() == 0:
			game = Game(name=request['game'].decode('utf-8'))
			game.put()
			ostr += "Game added.<br />"
		else:
			ostr += "Game already exists.<br />"
	if request['team']:
		if Team.all().filter('name =', request['team'].decode('utf-8')).count() == 0:
			team = Team(name=request['team'].decode('utf-8'))
			team.put()
			ostr += "Team added.<br />"
		else:
			ostr += "Team already exists.<br />"
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
