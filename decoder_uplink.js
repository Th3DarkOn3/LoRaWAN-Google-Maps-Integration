function decodeUplink(input) {
  const bytes = input.bytes;
  
  const latitude = ((bytes[8] << 16) | (bytes[9] << 8) | bytes[10]);
  const longitude = ((bytes[11] << 16) | (bytes[12] << 8) | bytes[13]);

  // Convertir d’entier en float
  const lat = latitude / 10000;
  const lon = longitude / 10000;

  return {
    data: {
      led: bytes[0],
      pressure: (bytes[1] << 8) | bytes[2],
      temperature: ((bytes[3] << 8) | bytes[4]) / 100,
      humidity: ((bytes[5] << 8) | bytes[6]) / 10,
      battery: bytes[7],
      latitude: lat,
      longitude: lon,
      altitude: (bytes[14] << 8) | bytes[15]
    }
  };
}
