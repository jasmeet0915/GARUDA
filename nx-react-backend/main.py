from flask import Flask, request, jsonify

app = Flask(__name__)

# In-memory storage for animal locations and types
animal_locations = []
animal_types = []


@app.route('/update_location', methods=['POST'])
def update_location():
    data = request.json
    animal_type = data.get('animal_type')
    location = data.get('location')
    camera = data.get('camera')

    if not animal_type or not location or not camera:
        return jsonify({"error": "animal_type, location, and camera are required"}), 400

    if animal_type not in ['elephant', 'giraffe', 'cattle', 'zebra']:
        return jsonify({"error": "Invalid animal type"}), 400

    animal_locations.append({
        "animal_type": animal_type,
        "location": location,
        "camera": camera
    })

    return jsonify({"message": "Location and camera updated successfully"}), 200


@app.route('/update_animal_type', methods=['POST'])
def update_animal_type():
    data = request.json
    animal_type = data.get('animal_type')
    camera = data.get('camera')

    if not animal_type or not camera:
        return jsonify({"error": "animal_type and camera are required"}), 400

    if animal_type not in ['elephant', 'giraffe', 'cattle', 'zebra']:
        return jsonify({"error": "Invalid animal type"}), 400

    animal_types.append({
        "animal_type": animal_type,
        "camera": camera
    })

    return jsonify({"message": "Animal type and camera updated successfully"}), 200


@app.route('/get_locations', methods=['GET'])
def get_locations():
    return jsonify(animal_locations), 200


@app.route('/get_animal_types', methods=['GET'])
def get_animal_types():
    return jsonify(animal_types), 200


if __name__ == '__main__':
    app.run(debug=True)
