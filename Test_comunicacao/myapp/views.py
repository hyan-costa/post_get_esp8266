import datetime
import json

from django.shortcuts import render, HttpResponse
from django.views.decorators.csrf import csrf_exempt

@csrf_exempt
def get_data(request):
    if request.method=="POST":
        response = {}
        json_uid = request.body.decode("utf-8")
        uid = json.loads(json_uid)
        if uid["uid"]:
            params = {
                'result':True
            }
            params_json = json.dumps(params)
            response = HttpResponse(params_json, content_type='application/json')

        return response
    if request.method=="GET":
        print(request)
        return render(request,'test.html')