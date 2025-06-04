let map;

window.initMap = function () {
  console.log("Fonction initMap appelée");

  map = new google.maps.Map(document.getElementById("map"), {
    center: { lat: 46.603354, lng: 1.888334 },
    zoom: 6,
    mapId: 'adc3a7e5bb31204643eebbd6'
  });

  function chargerPoints() {
  fetch('../location.json')
    .then(response => {
      if (!response.ok) {
        throw new Error("Erreur de chargement JSON");
      }
      return response.json();
    })
    .then(data => {
      console.log("Données JSON :", data);

      data.forEach(point => {
        const { AdvancedMarkerElement } = google.maps.marker;

        new AdvancedMarkerElement({
          position: { lat: point.lat, lng: point.lng },
          map: map,
          title: point.label
        });
      });

      if (data.length > 0) {
        const lastPoint = data[data.length - 1];
        map.setCenter({ lat: lastPoint.lat, lng: lastPoint.lng });
      }
    })
    .catch(error => {
      console.error("Erreur JSON :", error);
    });
}
  chargerPoints();

  // Connexion WebSocket
  const ws = new WebSocket("ws://localhost:8765");
  ws.onmessage = function(event) {
    if (event.data === "reload") {
      // Recharge les points
      chargerPoints();
    }
  };
}