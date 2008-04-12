from django.conf.urls.defaults import *
from live.views import *
from admin.views import *
from grab.views import *

urlpatterns = patterns('',
    # Example:
    (r'^/?$', live),
	(r'^game/(.+?)/$', listMatches),
	(r'^channel/(.+?)/$', listMethods),
	(r'^go/(.+)/(.+?)/$', actMethod),

    # simple admin
	(r'^admin$', console),
	(r'^admin/add/$', addEntry),
	(r'^admin/addMethod/$', addMethod),
	(r'^admin/addMatch/$', addMatch),
	
	# grab
	(r'^grab$', getZhiBoBa),
	
)
