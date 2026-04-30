Fichiers de description JSON pour les Spritesheets
==================================================

Ce guide présente en détail la structure des fichiers de description JSON utilisés pour charger et découper les spritesheets (atlas de textures).
Ces fichiers JSON permettent de définir comment une image est découpée en sprites individuels, et comment ces sprites sont organisés en groupes pour créer des animations.

--------------------------------
# Introduction

Dans le moteur, chaque spritesheet (fichier image, souvent en PNG) est accompagnée d'un **fichier de description JSON** portant le suffixe `_desc.json`.
Par exemple, pour l'image `player.png`, le fichier de description sera `player_desc.json`.

Ces fichiers de description servent à :
- Définir la **géométrie** du découpage de la texture (grille régulière ou rectangles personnalisés)
- Organiser les sprites en **groupes nommés** utilisables pour les animations
- Spécifier des paramètres de rendu (pixel art, bordures 9-slice...)
- Optimiser le chargement en évitant de recalculer manuellement les coordonnées de chaque sprite

Le parser de ces fichiers se trouve dans `engine/src/core/sprite_sheet.c` pour plus de détails.

--------------------------------
# Structure de base d'un fichier JSON

Un fichier de description JSON contient plusieurs sections principales :

```json
{
  "pixelArt": true,
  "geometry": { ... },
  "rectangles": [ ... ],
  "borders": [ ... ],
  "groups": [ ... ]
}
```

Détaillons chacune de ces sections.

--------------------------------
# Section `pixelArt`

**Type** : `boolean` (optionnel)  
**Valeur par défaut** : `false`

Indique si la texture doit être rendue en mode pixel art (sans filtrage bilinéaire).

```json
{
  "pixelArt": true
}
```

Lorsque `pixelArt` est à `true`, SDL3 utilisera un filtrage "nearest neighbor" qui préserve les pixels nets lors de l'agrandissement.
Ceci est essentiel pour les graphismes rétro en basse résolution.

--------------------------------
# Définition de la géométrie : deux approches

Il existe deux méthodes pour définir le découpage d'une spritesheet : la **géométrie régulière** (grille) et les **rectangles personnalisés**.
Ces deux approches sont **mutuellement exclusives** : vous devez choisir l'une ou l'autre.

## Approche 1 : Géométrie régulière (`geometry`)

Cette approche est utilisée lorsque la spritesheet est organisée en **grille régulière** avec des sprites de taille identique.

### Structure de base

```json
{
  "geometry": {
    "rowCount": 8,
    "colCount": 8,
    "width": 48,
    "height": 48,
    "spacing": 0,
    "padding": 0
  }
}
```

### Paramètres requis

| Paramètre | Type | Description |
|-----------|------|-------------|
| `rowCount` | `int` | Nombre de lignes de sprites dans la grille |
| `colCount` | `int` | Nombre de colonnes de sprites dans la grille |
| `width` | `int` | Largeur en pixels d'un sprite individuel |
| `height` | `int` | Hauteur en pixels d'un sprite individuel |

### Paramètres optionnels

| Paramètre | Type | Défaut | Description |
|-----------|------|--------|-------------|
| `spacing` | `int` | `0` | Espacement horizontal et vertical entre les sprites |
| `spacingX` | `int` | `0` | Espacement horizontal entre les sprites (prioritaire sur `spacing`) |
| `spacingY` | `int` | `0` | Espacement vertical entre les sprites (prioritaire sur `spacing`) |
| `padding` | `int` | `0` | Marge extérieure (horizontal et vertical) avant le premier sprite |
| `paddingX` | `int` | `0` | Marge extérieure horizontale (prioritaire sur `padding`) |
| `paddingY` | `int` | `0` | Marge extérieure verticale (prioritaire sur `padding`) |

### Exemple détaillé

```json
{
  "geometry": {
    "rowCount": 3,
    "colCount": 8,
    "spacing": 0,
    "padding": 0,
    "width": 64,
    "height": 64
  }
}
```

Ce fichier décrit une grille de 3 lignes × 8 colonnes = **24 sprites au total**.
Chaque sprite mesure 64×64 pixels, sans espacement entre eux.

Les sprites sont numérotés de gauche à droite, de haut en bas : le sprite en haut à gauche est l'index `0`, celui à sa droite est l'index `1`, etc.

### Exemple avec espacement et marges

```json
{
  "geometry": {
    "rowCount": 1,
    "colCount": 5,
    "width": 6,
    "height": 6,
    "spacing": 2,
    "padding": 1
  }
}
```

Ce fichier décrit une ligne horizontale de 5 sprites de 6×6 pixels :
- Décalage initial de 1 pixel sur les bords (`padding`)
- Espacement de 2 pixels entre chaque sprite (`spacing`)

Le premier sprite commence donc à la position (1, 1), le deuxième à (1+6+2, 1) = (9, 1), etc.

## Approche 2 : Rectangles personnalisés (`rectangles`)

Cette approche est utilisée pour des spritesheets avec des sprites de **tailles variables** ou organisés de manière **irrégulière**.

### Structure

```json
{
  "rectangles": [
    {"x": 0, "y": 0, "w": 4, "h": 16},
    {"x": 4, "y": 0, "w": 4, "h": 16},
    {"x": 8, "y": 0, "w": 4, "h": 16},
    {"x": 0, "y": 16, "w": 8, "h": 16}
  ]
}
```

### Paramètres de chaque rectangle

| Paramètre | Type | Description |
|-----------|------|-------------|
| `x` | `int` | Position X du coin supérieur gauche dans la texture (en pixels) |
| `y` | `int` | Position Y du coin supérieur gauche dans la texture (en pixels) |
| `w` | `int` | Largeur du sprite (en pixels) |
| `h` | `int` | Hauteur du sprite (en pixels) |
| `comment` | `string` | (Optionnel) Commentaire pour identifier le sprite |

### Exemple complet

```json
{
  "pixelArt": true,
  "rectangles": [
    {"x": 0, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 0"},
    {"x": 4, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 1"},
    {"x": 8, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 2"},
    {"x": 12, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 3"},
    
    {"x": 0, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 0"},
    {"x": 8, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 1"},
    {"x": 16, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 2"},
    
    {"x": 32, "y": 0, "w": 16, "h": 32, "comment": "torpedo frame 0"},
    {"x": 48, "y": 0, "w": 16, "h": 32, "comment": "torpedo frame 1"}
  ]
}
```

Les rectangles sont indexés dans l'ordre de leur déclaration : le premier rectangle a l'index `0`, le deuxième l'index `1`, etc.

> [!WARNING]
> Les paramètres `comment` ne sont utilisés que pour la documentation et ne sont **pas lus par le parser**.

--------------------------------
# Section `borders` : 9-slice pour les interfaces

**Type** : `array` (optionnel)

La section `borders` permet de définir des sprites avec des **bordures 9-slice**, une technique utilisée pour créer des éléments d'interface utilisateur redimensionnables (boutons, barres de vie, panneaux...).
Le principe du 9-slice consiste à diviser un sprite en 9 zones : 4 coins (taille fixe), 4 bords (étirables dans une direction) et 1 centre (étirable dans les deux directions).

### Structure

```json
{
  "borders": [
    {
      "left": 2,
      "right": 2,
      "top": 2,
      "bottom": 2,
      "scale": 2,
      "frames": [0, 1, 2, 3, 4]
    }
  ]
}
```

### Paramètres

| Paramètre | Type | Défaut | Description |
|-----------|------|--------|-------------|
| `left` | `int` | `0` | Largeur de la bordure gauche (en pixels) |
| `right` | `int` | `0` | Largeur de la bordure droite (en pixels) |
| `top` | `int` | `0` | Hauteur de la bordure supérieure (en pixels) |
| `bottom` | `int` | `0` | Hauteur de la bordure inférieure (en pixels) |
| `scale` | `int` | `0` | Facteur d'échelle appliqué aux bordures |
| `frames` | `array[int]` | - | Liste des indices de sprites auxquels appliquer ces bordures |

### Exemple : barre de vie

```json
{
  "pixelArt": true,
  "geometry": {
    "rowCount": 1,
    "colCount": 5,
    "spacing": 2,
    "padding": 1,
    "width": 6,
    "height": 6
  },
  "borders": [
    {
      "left": 2,
      "right": 2,
      "top": 2,
      "bottom": 2,
      "scale": 2,
      "frames": [0, 1, 2, 3, 4]
    }
  ]
}
```

Cet exemple définit une barre de vie avec 5 sprites différents (cadre vide, remplissage à 25%, 50%, 75%, 100%).
Chaque sprite peut être redimensionné en préservant ses bordures de 2 pixels de chaque côté, avec un facteur d'échelle de 2.

--------------------------------
# Section `groups` : organisation des animations

**Type** : `array` (optionnel mais fortement recommandé)

La section `groups` permet d'organiser les sprites en **groupes nommés**, généralement pour définir des animations ou des états visuels. Chaque groupe contient une liste d'indices de sprites qui seront joués dans l'ordre pour créer une animation.

Un groupe peut aussi servir à regrouper des sprites statiques. Il peut par exemple contenir toutes les tuiles de représentant le sol pour une tilemap d'un platformer.

### Structure

```json
{
  "groups": [
    {
      "name": "idle",
      "frames": [0]
    },
    {
      "name": "firing",
      "frames": [0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5]
    },
    {
      "name": "dying",
      "frames": [16, 17, 18, 19, 20, 21, 22, 23]
    }
  ]
}
```

### Paramètres

| Paramètre | Type | Description |
|-----------|------|-------------|
| `name` | `string` | Nom du groupe (utilisé dans le code pour référencer le groupe) |
| `frames` | `array[int]` | Liste ordonnée des indices de sprites composant ce groupe |

### Utilisation dans le code

Dans le code C, vous pouvez récupérer un groupe par son nom :

```c
SpriteGroup* idleGroup = SpriteSheet_getGroupByName(spriteSheet, "idle");
SpriteGroup* firingGroup = SpriteSheet_getGroupByName(spriteSheet, "firing");
```

### Techniques avancées

#### Répétition de frames

Il est possible de répéter des frames dans un groupe pour créer des **ralentissements** ou des **pauses** dans l'animation :

```json
{
  "name": "firing",
  "frames": [0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5]
}
```

Ici, la frame `5` est répétée 6 fois, ce qui la maintient affichée plus longtemps.

#### Groupes à frame unique

Un groupe avec une seule frame peut être utilisé pour des sprites statiques :

```json
{
  "name": "ship",
  "frames": [0]
}
```

#### Organisation logique

Les groupes permettent d'organiser logiquement les sprites selon leur fonction :

```json
{
  "groups": [
    {
      "name": "ship",
      "frames": [0]
    },
    {
      "name": "engine",
      "frames": [1, 2, 3, 4]
    },
    {
      "name": "auto-canons",
      "frames": [8, 9, 10, 11, 12, 13, 14]
    }
  ]
}
```

Ce découpage permet de contrôler séparément l'animation du vaisseau principal, de ses moteurs et de ses armes.

--------------------------------
# Exemples complets

## Exemple 1 : Vaisseau du joueur avec géométrie régulière

**Fichier** : `player_desc.json`  
**Texture** : `player.png` (384×384 pixels, grille 8×8)

```json
{
  "pixelArt": true,
  "geometry": {
    "rowCount": 8,
    "colCount": 8,
    "spacing": 0,
    "width": 48,
    "height": 48
  },
  "groups": [
    {
      "name": "ship",
      "frames": [0]
    },
    {
      "name": "engine",
      "frames": [1, 2, 3, 4]
    },
    {
      "name": "auto-canons",
      "frames": [8, 9, 10, 11, 12, 13, 14]
    },
    {
      "name": "big-gun",
      "frames": [
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28
      ]
    },
    {
      "name": "rockets",
      "frames": [
        32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47
      ]
    }
  ]
}
```

**Explication** :
- Grille régulière de 8×8 = 64 sprites de 48×48 pixels
- 5 groupes pour contrôler séparément chaque partie du vaisseau
- Le groupe `ship` contient le corps principal (statique)
- Les autres groupes contiennent les animations des armes

## Exemple 2 : Ennemi avec géométrie et animations

**Fichier** : `fighter_desc.json`  
**Texture** : `fighter.png` (512×192 pixels, grille 8×3)

```json
{
  "pixelArt": true,
  "geometry": {
    "rowCount": 3,
    "colCount": 8,
    "spacing": 0,
    "width": 64,
    "height": 64
  },
  "groups": [
    {
      "name": "idle",
      "frames": [0]
    },
    {
      "name": "firing",
      "frames": [0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5]
    },
    {
      "name": "shield",
      "frames": [6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
    },
    {
      "name": "dying",
      "frames": [16, 17, 18, 19, 20, 21, 22, 23]
    }
  ]
}
```

**Explication** :
- Grille régulière de 8×3 = 24 sprites de 64×64 pixels
- 4 états : repos, tir, bouclier activé, destruction
- L'animation `firing` maintient la dernière frame pour l'effet de recul
- Toutes les animations peuvent être jouées séparément selon l'état de l'ennemi

## Exemple 3 : Projectiles avec rectangles personnalisés

**Fichier** : `bullets_desc.json`  
**Texture** : `bullets.png` (tailles variables)

```json
{
  "pixelArt": true,
  "rectangles": [
    {"x": 0, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 0"},
    {"x": 4, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 1"},
    {"x": 8, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 2"},
    {"x": 12, "y": 0, "w": 4, "h": 16, "comment": "basic-1 frame 3"},
    
    {"x": 0, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 0"},
    {"x": 8, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 1"},
    {"x": 16, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 2"},
    {"x": 24, "y": 16, "w": 8, "h": 16, "comment": "basic-2 frame 3"},

    {"x": 32, "y": 0, "w": 16, "h": 32, "comment": "torpedo frame 0"},
    {"x": 48, "y": 0, "w": 16, "h": 32, "comment": "torpedo frame 1"},
    {"x": 64, "y": 0, "w": 16, "h": 32, "comment": "torpedo frame 2"}
  ],
  "groups": [
    {
      "name": "basic-1",
      "frames": [0, 1, 2, 3]
    },
    {
      "name": "basic-2",
      "frames": [4, 5, 6, 7]
    },
    {
      "name": "torpedo",
      "frames": [8, 9, 10]
    }
  ]
}
```

**Explication** :
- Rectangles personnalisés car les projectiles ont des tailles différentes
- Le projectile `basic-1` fait 4×16 pixels
- Le projectile `basic-2` fait 8×16 pixels
- La torpille `torpedo` fait 16×32 pixels
- Chaque type de projectile a son propre groupe d'animation

## Exemple 4 : Barre de vie avec bordures 9-patch

**Fichier** : `health_bar_desc.json`  
**Texture** : `health_bar.png` (petite texture 6×6 par sprite)

```json
{
  "pixelArt": true,
  "geometry": {
    "rowCount": 1,
    "colCount": 5,
    "spacing": 2,
    "padding": 1,
    "width": 6,
    "height": 6
  },
  "borders": [
    {
      "left": 2,
      "right": 2,
      "top": 2,
      "bottom": 2,
      "scale": 2,
      "frames": [0, 1, 2, 3, 4]
    }
  ],
  "groups": [
    {
      "name": "borders",
      "frames": [0]
    },
    {
      "name": "fill",
      "frames": [1, 2, 3, 4]
    }
  ]
}
```

**Explication** :
- 5 sprites de 6×6 pixels espacés de 2 pixels avec une marge de 1 pixel
- Tous les sprites ont des bordures 9-patch de 2 pixels
- Le groupe `borders` contient le cadre vide
- Le groupe `fill` contient 4 niveaux de remplissage (vert, jaune, orange, rouge)
- Le facteur d'échelle de 2 multiplie la taille des bordures lors du rendu

--------------------------------
# Bonnes pratiques

## Organisation des fichiers

- **Nommage** : Utilisez toujours le suffixe `_desc.json` pour les fichiers de description
- **Emplacement** : Placez les fichiers JSON à côté de leurs textures PNG correspondantes
- **Cohérence** : Utilisez des noms de groupes cohérents entre différentes spritesheets (ex : "idle", "dying", "firing")

## Choix de la méthode de découpage

- **Géométrie régulière** : À privilégier pour les spritesheets organisées en grille (plus simple, plus lisible)
- **Rectangles personnalisés** : Nécessaire pour les sprites de tailles variables ou les atlas optimisés (moins d'espace perdu)

## Optimisation

- **Regroupement** : Placez plusieurs sprites dans une même texture pour réduire les changements de texture (meilleure performance)
- **Espacement** : Ajoutez 1-2 pixels d'espacement entre les sprites pour éviter les artefacts de filtrage

## Documentation

- **Commentaires** : Utilisez le champ `comment` dans les rectangles pour documenter votre spritesheet
- **Organisation logique** : Ordonnez les rectangles de manière logique (par type, par animation...)
- **Noms explicites** : Choisissez des noms de groupes clairs et descriptifs

--------------------------------
# Débogage

Pour vérifier que votre fichier JSON est correct :

1. **Syntaxe JSON** : Utilisez un validateur JSON en ligne ou l'extension JSON de VSCode
2. **Calcul manuel** : Vérifiez que `rowCount × colCount` correspond au nombre de sprites attendu
3. **Indices** : Vérifiez que tous les indices dans les groupes sont valides (< nombre total de sprites)
4. **Coordonnées** : Pour les rectangles, vérifiez que `x + w` et `y + h` ne dépassent pas les dimensions de la texture

--------------------------------
# Références

- **Parser** : `engine/src/core/sprite_sheet.c`
- **Structures** : `engine/include/core/sprite_sheet.h`
- **Exemples** : `assets_dev/atlas/*_desc.json`