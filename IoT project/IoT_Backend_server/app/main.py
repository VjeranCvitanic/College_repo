from fastapi import FastAPI, HTTPException
import requests
from supabase import create_client, Client

# Configuration for Home Assistant 
HOMEASSISTANT_URL = "https://najjacagrupa.ninja"
API_PASSWORD = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJmNTU3YzNjOWY0N2Q0NmFjOWVlYjY3ZDRjMTU2OWM4NyIsImlhdCI6MTcxNzUxNzk0NywiZXhwIjoyMDMyODc3OTQ3fQ.3GHy5DYqEDIoc8SzXiWnBRRwVH1qvIlM7irDromKHkQ"

# Configuration for Supabase
SUPABASE_URL = "https://gguxaxulmcemrqatwkge.supabase.co"
SUPABASE_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImdndXhheHVsbWNlbXJxYXR3a2dlIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MTc3MTI2OTgsImV4cCI6MjAzMzI4ODY5OH0.-dfDakyaHVbIt4EmVfewTggbX3qhUiJkPsFHahTE4TM"

supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)
app = FastAPI()


# GET
# "/locations" - get all locations and their rooms
# "/sensor_data/{location}/{room}" - get sensor data for a specific room

# Helper function to make authenticated requests to HomeAssistant
def call_ha_api(method: str, endpoint: str, json: dict = None):
    url = f"{HOMEASSISTANT_URL}/api{endpoint}"
    headers = {"Authorization": f"Bearer {API_PASSWORD}"}
    try:
        if method == "GET":
            response = requests.get(url, headers=headers, timeout=10)
        else:
            raise NotImplementedError(f"Method {method} not supported")
        response.raise_for_status()
        return response.json()
    except requests.exceptions.HTTPError as http_err:
        raise HTTPException(status_code=response.status_code, detail=str(http_err))
    except requests.exceptions.ConnectTimeout as timeout_err:
        raise HTTPException(status_code=504, detail=f"Connection timed out: {str(timeout_err)}")
    except Exception as err:
        raise HTTPException(status_code=500, detail=str(err))

"""# Path operation to get all states
@app.get("/all")
async def get_states():
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    for elem in states:
        partial_data = {
            "entity_id": elem.get("entity_id"),
            "state": elem.get("state"),
        }
        print(partial_data)  # Print the filtered partial data
    
    return 0


@app.get("/")
async def get_states():
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)
    allowed_types = ["counter", "sensor", "binary_sensor", "automation", "weather"]  # Example types

    filtered_entities = []

    for elem in states:
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            partial_data = {
                "entity_id": entity_id,
                "state": elem.get("state"),
            }
            filtered_entities.append(partial_data)
            print(partial_data)  # Print the filtered partial data
    
    return filtered_entities


@app.get("/sensor")
async def get_states():
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    allowed_types = ["sensor"]  # Example types

    filtered_entities = []

    for elem in states:
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            partial_data = {
                "entity_id": entity_id,
                "state": elem.get("state"),
            }
            filtered_entities.append(partial_data)
            print(partial_data)  # Print the filtered partial data
    
    return filtered_entities

@app.get("/chair")
async def get_states():
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    allowed_types = ["binary_sensor"]  # Example types

    filtered_entities = []

    for elem in states:
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            partial_data = {
                "entity_id": entity_id,
                "state": elem.get("state"),
            }
            filtered_entities.append(partial_data)
            print(partial_data)  # Print the filtered partial data
    
    return filtered_entities

@app.get("/all_locations")
async def get_states():
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    allowed_types = ["zone"]  # Example types
    print("states",states)
    locations = []
    for elem in states:
        print("elem",elem)
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            entity_id = entity_id.split(".")[1]
            partial_data = {
                "entity_id": entity_id,
            }

            locations.append(partial_data)
    
    return locations

@app.get("/start_info")
async def get_states():
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    allowed_types = ["zone"]  # Example types

    locations = []

    for elem in states:
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            entity_id = entity_id.split(".")[1]

            locations.append(entity_id)
    
    info = []
    for location in locations:
        loc_info = []
        loc_info.append(location)
        loc_info.append(get_location_info(location))
        info.append(loc_info)

    print(info)
    return info

def get_location_info(location):
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    allowed_types = ["counter"]  # Example types

    #rooms in this location
    rooms = []
    rooms_info = []

    for elem in states:
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            # Split entity_id by underscores
            parts = entity_id.split("_")
            if len(parts) >= 2:
                # Extract the location from the first part
                entity_location = parts[0].split(".")[1]  # Extract location from "counter.location1"
                # Check if the entity's location matches the provided location
                if entity_location == location:
                    entity_id = parts[1]
                    partial_data = {
                        "room_id": entity_id,
                    }
                    rooms.append(partial_data)

    for room in rooms:
        room_info = []
        room_info.append(room)
        room_info.append(get_room_info(location, room.get("room_id")))

        rooms_info.append(room_info)
    
    return json.dumps(rooms_info)

@app.get("/{location}/{room}")
def get_room_info(location, room):
    endpoint = "/states"
    #return {"message": "Welcome to the Home Assistant API integration"}
    states = call_ha_api("GET", endpoint)

    allowed_types = ["counter", "sensor", "binary_sensor"]  # Example types

    #rooms in this location
    room_info = []

    for elem in states:
        entity_id = elem.get("entity_id")
        
        # Check if the entity_id starts with any of the allowed types
        if any(entity_id.startswith(allowed_type + ".") for allowed_type in allowed_types):
            # Split entity_id by underscores
            parts = entity_id.split("_")
            if len(parts) >= 2:
                # Extract the location from the first partž
                try:
                    entity_location = parts[0].split(".")[1]  # Extract location from "counter.location1"
                    room_id = parts[1]
                except:
                    entity_location = parts[1].split(".")[1]  # Extract location from "counter.location1"
                    room_id = parts[2]
                # Check if the entity's location matches the provided location
                if entity_location == location:
                    if(room_id == room):
                        try:
                            entity_id = parts[3]
                        except:
                            entity_id = parts[2]
                        partial_data = {
                            "entity_id": entity_id,
                            "state": elem.get("state"),
                        }
                        room_info.append(partial_data)

    return room_info
"""
    
@app.get("/locations")
def get_locations():
    try:
        response = supabase.table("locations").select("id,name,code").execute()

        for location in response.data:
            location_id = location.get("id")
            response_rooms = supabase.table("rooms").select("id,name,code,description,layout").eq("location_id",location_id).execute()
            location["rooms"] = response_rooms.data

        print(response.data)

        return response.data
    except Exception as e:
        print(str(e))
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/sensor_data/{location}/{room}")
def get_sensor_data(location, room):
    try:
        # read from home assistant
        endpoint = "/states"
        states = call_ha_api("GET", endpoint)

        sensor_data = {}
        chairs = {}

        location_room = location + "_" + room

        for elem in states:
            #print("elem",elem)
            entity_id_useful = str(elem.get("entity_id").split(".")[1])
            if entity_id_useful.startswith(location_room):
               # split by _, join 0th and 1th, and join all the rest
                sensorType = str(entity_id_useful[len(location_room) + 1:])

                if sensorType == "stolica_free":
                    sensor_data["free_chairs"] = elem.get("state")
                elif sensorType == "ac":
                    sensor_data["ac"] = elem.get("state")
                elif sensorType == "humidity":
                    sensor_data["humidity"] = round(float(elem.get("state")),0)
                elif sensorType == "temperature":
                    sensor_data["temperature"] = round(float(elem.get("state")),1)
                elif sensorType == "pressure":
                    sensor_data["pressure"] = round(float(elem.get("state")),0)
                elif sensorType.startswith("stolica"):
                    chairs[sensorType] =  elem.get("state")
        
        sensor_data["chairs"] = chairs

        print(sensor_data)
        return sensor_data

    except Exception as e:
        print(str(e))
        raise HTTPException(status_code=500, detail=str(e))
