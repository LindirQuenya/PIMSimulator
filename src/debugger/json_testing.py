import json

data = {"type": "b", "data": ""}

print(data)
temp_Data = json.dumps(data)
json_data = json.loads(temp_Data)
print(json_data)
command = json_data.get("type")
print(command)