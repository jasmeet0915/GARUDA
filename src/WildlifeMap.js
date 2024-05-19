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

// Function to calculate the position on a circular path
const calculatePosition = (centerX, centerY, radius, angle) => {
    const x = centerX + radius * Math.cos(angle);
    const y = centerY + radius * Math.sin(angle);
    return [x, y];
};

// Function to move an animal in a slow random path
const moveAnimal = (animal, speed) => {
    const [animalPos, animalAngle, animalSpeed, animalType] = animal;
    const newAngle = animalAngle + (Math.random() - 0.5) * speed; // Adding small randomness to the movement
    const [newX, newY] = [animalPos[0] + Math.cos(newAngle) * speed, animalPos[1] + Math.sin(newAngle) * speed];
    return [[newX, newY], newAngle, animalSpeed, animalType];
};

const WildlifeMap = () => {
    const [dronePosition, setDronePosition] = useState([500, 900]); // Initial position of the drone
    const [animalStates, setAnimalStates] = useState([
        [[450, 850], 0, 0.0, 'elephant'], // Initial animals
        [[550, 950], 0, 0.0, 'zebra']
    ]); // Positions and states of the animals
    const [angle, setAngle] = useState(0); // Initial angle for circular path
    const requestRef = useRef();

    useEffect(() => {
        const animate = () => {
            // Update the angle
            const newAngle = (angle + 0.005) % (2 * Math.PI); // Faster circular movement
            setAngle(newAngle);

            // Calculate new drone position
            const newDronePosition = calculatePosition(500, 900, 200, newAngle); // Center (500, 900), radius 200
            setDronePosition(newDronePosition);

            // Update animal positions
            const updatedAnimalStates = animalStates.map(animal => moveAnimal(animal, 0.0)); // Initial animals do not move

            // Spawn animals based on drone's angle
            if (newAngle > Math.PI / 4 && newAngle < Math.PI / 4 + 0.01 && animalStates.length < 4) {
                updatedAnimalStates.push([calculatePosition(500, 900, 250, newAngle), 0, 0.1, 'cattle']);
            }
            if (newAngle > Math.PI / 2 && newAngle < Math.PI / 2 + 0.01 && animalStates.length < 5) {
                updatedAnimalStates.push([calculatePosition(500, 900, 250, newAngle), 0, 0.1, 'zebra']);
            }

            // Move one stationary animal after drone crosses a certain point
            if (newAngle > Math.PI && newAngle < Math.PI + 0.01 && animalStates.length === 5) {
                updatedAnimalStates[0] = moveAnimal(animalStates[0], 0.0005); // Move the first animal slightly
            }

            setAnimalStates(updatedAnimalStates);
            requestRef.current = requestAnimationFrame(animate);
        };

        requestRef.current = requestAnimationFrame(animate);
        return () => cancelAnimationFrame(requestRef.current);
    }, [angle, animalStates]);

    return (
        <MapContainer
            center={[500, 900]}
            zoom={-2} // Set an appropriate default zoom level to fit the map in the viewport
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
                            zebraMarker
                } />
            ))}
        </MapContainer>
    );
};

export default WildlifeMap;
