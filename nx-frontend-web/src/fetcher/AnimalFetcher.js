import axios from 'axios';

class AnimalDataFetcher {
    constructor() {
        this.locations = [];
        this.types = [];
        this.combinedData = [];
        this.locationsFetched = false;
        this.typesFetched = false;
    }

    async fetchLocations() {
        try {
            const response = await axios.get('http://localhost:5000/get_locations');
            this.locations = response.data;
            this.locationsFetched = true;
            this.combineData();
        } catch (error) {
            console.error('Error fetching locations:', error);
        }
    }

    async fetchTypes() {
        try {
            const response = await axios.get('http://localhost:5000/get_animal_types');
            this.types = response.data;
            this.typesFetched = true;
            this.combineData();
        } catch (error) {
            console.error('Error fetching types:', error);
        }
    }

    combineData() {
        if (this.locationsFetched && this.typesFetched) {
            this.combinedData = this.locations.map(location => {
                const type = this.types.find(t => t.camera === location.camera);
                return {
                    ...location,
                    ...type
                };
            });
        }
    }

    getCombinedData() {
        if (!this.locationsFetched || !this.typesFetched) {
            console.warn('Data has not been fully fetched and combined yet.');
            return [];
        }
        return this.combinedData;
    }

    async fetchData() {
        await Promise.all([this.fetchLocations(), this.fetchTypes()]);
    }
}

export default AnimalDataFetcher;
