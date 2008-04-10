from django.conf.urls.defaults import *
from live.views import *
from admin.views import *

urlpatterns = patterns('',
    # Example:
    (r'^/?$', live),
	(r'^game/(.+?)/$', listMatches),
	(r'^channel/(.+?)/$', listMethods),
	(r'^go/(.+)/(.+?)/$', actMethod),

    # Uncomment this for admin:
     (r'^admin/$', console),
     (r'^admin/add/$', addEntry),
     (r'^admin/addMethod/$', addMethod),
)
