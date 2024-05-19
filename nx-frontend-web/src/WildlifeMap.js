import React, { useEffect, useState, useRef } from 'react';
import { MapContainer, ImageOverlay, Marker } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import './App.css'; // Import the CSS file

const bounds = [[0, 0], [1000, 1800]];  // Adjust based on your image size and requirements

const droneMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/drone.png`,
    iconSize: [120, 120],
});

const elephantMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/elephant_icon.png`,
    iconSize: [100, 100],
});

const cattleMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/cattle.png`,
    iconSize: [80, 80],
});

const zebraMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/zebra.png`,
    iconSize: [80, 80],
});

const giraffeMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/giraffe.png`,
    iconSize: [90, 90],
});

const cameraMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/camera.png`,
    iconSize: [55, 55],
});

const poacherMarker = new L.Icon({
    iconUrl: `${process.env.PUBLIC_URL}/poacher.png`,
    iconSize: [55, 55],
});

// Function to calculate the position on a circular path
const calculatePosition = (centerX, centerY, radius, angle) => {
    const x = centerX + radius * Math.cos(angle);
    const y = centerY + radius * Math.sin(angle);
    return [x, y];
};

// Function to move an animal in a slow random path
const moveAnimal = (animal, speed) => {
    const [animalPos, animalAngle, animalSpeed, animalType] = animal;
    const newAngle = animalAngle + (Math.random() - 0.5) * speed;
    const [newX, newY] = [animalPos[0] + Math.cos(newAngle) * speed, animalPos[1] + Math.sin(newAngle) * speed];
    return [[newX, newY], newAngle, animalSpeed, animalType];
};

const WildlifeMap = () => {
    const [dronePosition, setDronePosition] = useState([500, 900]);
    const [animalStates, setAnimalStates] = useState([
        [[450, 850], 0, 0.0, 'elephant'],
        [[550, 950], 0, 0.0, 'zebra'],
        [[550, 700], 0, 0.0, 'giraffe']
    ]);
    const [angle, setAngle] = useState(0);
    const requestRef = useRef();

    useEffect(() => {
        const animate = () => {
            // Update the angle
            const newAngle = (angle + 0.001) % (2 * Math.PI);  // Reduced angle increment for slower speed
            setAngle(newAngle);

            // Calculate new drone position
            const newDronePosition = calculatePosition(500, 900, 200, newAngle);
            setDronePosition(newDronePosition);

            // Update animal positions
            const updatedAnimalStates = animalStates.map(animal => moveAnimal(animal, 0.0));

            // Spawn animals based on drone's angle
            if (newAngle > Math.PI / 4 && newAngle < Math.PI / 4 + 0.01 && animalStates.length < 4) {
                updatedAnimalStates.push([calculatePosition(500, 900, 250, newAngle), 0, 0.1, 'elephant']);
            }
            if (newAngle > Math.PI / 2 && newAngle < Math.PI / 2 + 0.01 && animalStates.length < 5) {
                updatedAnimalStates.push([calculatePosition(500, 900, 250, newAngle), 0, 0.1, 'zebra']);
            }
            if (newAngle > (3 * Math.PI) / 4 && newAngle < (3 * Math.PI) / 4 + 0.01 && animalStates.length < 6) {
                updatedAnimalStates.push([calculatePosition(300, 850, 50, newAngle), 0, 0.1, 'giraffe']);
            }
            if (newAngle > (7 * Math.PI) / 4 && newAngle < (7 * Math.PI) / 4 + 0.01 && animalStates.length < 7) {
                updatedAnimalStates.push([[760, 800], 0, 0.0, 'cattle']);
            }
            if (newAngle > (7 * Math.PI) / 4 && newAngle < (7 * Math.PI) / 4 + 0.01 && animalStates.length < 8) {
                updatedAnimalStates.push([[750, 700], 0, 0.0, 'cattle']);
            }

            if (newAngle > Math.PI && newAngle < Math.PI + 0.01 && animalStates.length === 8) {
                updatedAnimalStates[0] = moveAnimal(animalStates[0], 0.0005);
            }

            setAnimalStates(updatedAnimalStates);
            requestRef.current = requestAnimationFrame(animate);
        };

        requestRef.current = requestAnimationFrame(animate);
        return () => cancelAnimationFrame(requestRef.current);
    }, [angle, animalStates]);

    // Find the giraffe position to place the static drone to its right
    const giraffeState = animalStates.find(animal => animal[3] === 'giraffe');
    const staticDronePosition = giraffeState ? [giraffeState[0][0], giraffeState[0][1] + 70] : [0, 0];
    const cameraPosition = giraffeState ? [giraffeState[0][0] + 70, giraffeState[0][1] - 160] : [0, 0];
    const poacherPosition = giraffeState ? [giraffeState[0][0] + 30, giraffeState[0][1] - 220] : [0, 0];

    return (
        <MapContainer
            center={[500, 900]}
            zoom={-2}
            scrollWheelZoom={false}
            style={{ height: '100vh', width: '100vw' }}
            crs={L.CRS.Simple}
        >
            <ImageOverlay
                url={`${process.env.PUBLIC_URL}/Map.webp`}
                bounds={bounds}
            />
            <Marker position={dronePosition} icon={droneMarker} />
            {animalStates.map((animal, index) => (
                <Marker key={index} position={animal[0]} icon={
                    animal[3] === 'elephant' ? elephantMarker :
                        animal[3] === 'cattle' ? cattleMarker :
                            animal[3] === 'zebra' ? zebraMarker :
                                giraffeMarker
                } />
            ))}
            {giraffeState && <Marker position={staticDronePosition} icon={droneMarker} />}
            {giraffeState && <Marker position={cameraPosition} icon={cameraMarker} />}
            {giraffeState && <Marker position={poacherPosition} icon={poacherMarker} />}
        </MapContainer>
    );
};

export default WildlifeMap;
