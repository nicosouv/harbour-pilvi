# Solutions OAuth pour Application Publique

## Problème
On ne peut pas inclure client_id/client_secret dans le code source d'une app publique.

## Solutions

### 1. Backend Proxy (RECOMMANDÉ)
Créer un backend simple qui gère l'OAuth :

**Avantages:**
- Client secret reste sur serveur sécurisé
- Peut gérer refresh tokens
- Contrôle sur l'usage
- Support multi-cloud centralisé

**Inconvénients:**
- Nécessite hébergement
- Maintenance serveur

**Architecture:**
```
App Sailfish → Backend Proxy → Google OAuth
                ↓
          Device Code Flow
```

### 2. OAuth 2.0 Device Flow (MEILLEUR pour Sailfish)
Google supporte le "Device Flow" pour devices limités :

**Avantages:**
- ✅ Pas de client_secret nécessaire !
- ✅ Parfait pour devices sans navigateur
- ✅ UX similaire à ce qu'on a déjà
- ✅ Pas de backend nécessaire
- ✅ Approuvé par Google pour apps natives

**Flow:**
1. App demande un device code
2. Affiche une URL + code utilisateur
3. User va sur google.com/device
4. Entre le code
5. App poll pour obtenir le token

**C'est LA solution pour Sailfish OS !**

### 3. PKCE Flow (OAuth 2.0 for Native Apps)
Extension OAuth pour apps natives :

**Avantages:**
- Standard OAuth pour mobile
- Pas de client_secret
- Sécurisé via code_verifier

**Inconvénients:**
- Nécessite un navigateur système
- Redirect URI complexe sur Sailfish

### 4. Client ID public (Google autorise)
Pour apps natives, Google permet d'utiliser juste client_id :

**Avantages:**
- Simple
- Client ID peut être public
- Pas de serveur

**Inconvénients:**
- Moins sécurisé
- Quotas partagés

## Recommandation : Device Flow

C'est la meilleure solution pour Sailfish OS !
