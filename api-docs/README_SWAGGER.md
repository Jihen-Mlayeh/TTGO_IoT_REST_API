# TTGO IoT REST API - Documentation Swagger/OpenAPI

## 📋 Vue d'ensemble

Votre API REST ESP32 dispose maintenant d'un endpoint de documentation OpenAPI 3.0 intégré.

## 🚀 Utilisation

### Option 1 : Accès direct au JSON (dans votre navigateur)

Ouvrez votre navigateur et accédez à :
```
http://[ADRESSE_IP_ESP32]/api-docs
```

Vous verrez la spécification OpenAPI complète en JSON.

### Option 2 : Visualisation avec Swagger Editor en ligne

1. Accédez à https://editor.swagger.io
2. Copiez le JSON depuis `http://[ADRESSE_IP_ESP32]/api-docs`
3. Collez-le dans l'éditeur Swagger
4. Vous verrez une belle documentation interactive !

### Option 3 : Utilisation avec Postman

1. Ouvrez Postman
2. Allez dans "Import"
3. Sélectionnez "Link"
4. Collez : `http://[ADRESSE_IP_ESP32]/api-docs`
5. Postman importera automatiquement toutes vos routes !

### Option 4 : Utilisation avec un client HTTP

```bash
# Linux/Mac
curl http://[ADRESSE_IP_ESP32]/api-docs

# Windows PowerShell
Invoke-WebRequest -Uri "http://[ADRESSE_IP_ESP32]/api-docs"
```

## 📚 Routes API disponibles

| Méthode | Route | Description |
|---------|-------|-------------|
| GET | `/sensors` | Liste tous les capteurs |
| GET | `/sensors/temperature` | Lecture température |
| GET | `/sensors/light` | Lecture lumière |
| POST | `/led/on` | Allumer la LED |
| POST | `/led/off` | Éteindre la LED |
| POST | `/led/toggle` | Basculer LED |
| POST | `/threshold/set?temp=30&light=50` | Définir seuils |
| GET | `/threshold` | Obtenir seuils |
| POST | `/mode/set?mode=AUTO-TEMP` | Changer mode |
| GET | `/status` | Status complet |
| GET | `/api-docs` | Documentation OpenAPI ✨ |

## 💡 Exemples d'utilisation

### Lire la température
```bash
curl http://192.168.1.100/sensors/temperature
```

### Allumer la LED
```bash
curl -X POST http://192.168.1.100/led/on
```

### Changer en mode AUTO-TEMP
```bash
curl -X POST "http://192.168.1.100/mode/set?mode=AUTO-TEMP"
```

### Définir des seuils
```bash
curl -X POST "http://192.168.1.100/threshold/set?temp=35&light=60"
```

## 🔧 Intégration dans vos projets

### Python
```python
import requests

# Récupérer la doc OpenAPI
response = requests.get('http://192.168.1.100/api-docs')
api_spec = response.json()
print(api_spec)
```

### JavaScript
```javascript
fetch('http://192.168.1.100/api-docs')
  .then(response => response.json())
  .then(spec => console.log(spec));
```

### Node.js avec Swagger UI Express
```javascript
const swaggerUi = require('swagger-ui-express');
const axios = require('axios');

app.use('/docs', async (req, res, next) => {
  const apiSpec = await axios.get('http://192.168.1.100/api-docs');
  swaggerUi.setup(apiSpec.data)(req, res, next);
});
```

## 🎯 Avantages

✅ **Léger** : Seulement ~3KB de code supplémentaire
✅ **Dynamique** : L'adresse IP est mise à jour automatiquement
✅ **Standard** : Compatible OpenAPI 3.0
✅ **Intégration facile** : Fonctionne avec Postman, Swagger UI, Insomnia, etc.

## 📝 Notes techniques

- Le JSON est généré dynamiquement à la volée
- Optimisé pour la mémoire limitée de l'ESP32
- Pas de fichier statique stocké sur l'ESP32
- Compatible avec tous les outils OpenAPI 3.0

## 🛠️ Personnalisation

Pour modifier la documentation, éditez la fonction `handleApiDocs()` dans `RestAPI.cpp`.

## 📞 Support

Si vous rencontrez des problèmes :
1. Vérifiez que votre ESP32 est connecté au WiFi
2. Confirmez que le serveur HTTP est démarré
3. Testez d'abord avec `/status` pour vérifier que l'API fonctionne
