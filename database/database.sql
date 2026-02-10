
CREATE TABLE sensor_data (
    id SERIAL PRIMARY KEY,
    device_id TEXT NOT NULL,
    temperature FLOAT,
    humidity FLOAT,
    luminosity FLOAT,
    created_at TIMESTAMP DEFAULT now()
);

SELECT * FROM sensor_data;
