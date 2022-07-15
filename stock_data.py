from keras.models import load_model
from pymongo import MongoClient
from serial import Serial
import re, os
import serial, time, schedule

def relative_to_absolute(path):
    dirname = os.path.dirname(__file__)
    filename = os.path.join(dirname, path)
    return filename

client = MongoClient("")              #Connect Cloud
arduino = serial.Serial('com4', 9600) #Connect Arduino Card
db = client["Smart"]                  #Database Name
document = db['Irrigation']           #Document Name

def Stock_data():       
    arduino_data = arduino.readline()
    pattern = r"Soil: (.*)%, Humidity: (.*) %, Temp: (.*) Celsius, Pluie: (.*)"
    soil, hum, temp, pluie = list(map(lambda x: float(x), re.search(pattern, arduino_data.decode("utf-8")).groups()))
    data = {"Soil":soil, "Temp":temp, "Hum":hum, "Pluie":pluie}
    # If necessary
    #if document.count() == 10: 
    #    _id = list(document.find().limit(1))[0]["_id"]
    #    document.remove({"_id": _id})
    document.insert_one(data)
    arduino_data = 0

schedule.every(2).seconds.do(Stock_data)
while True:
    schedule.run_pending()
    time.sleep(2)