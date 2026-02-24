@tool
class_name VERY_IMPORTANT_DATA_BASE

static var NAMES: Array[String] = [
	"Mika",
	"Javi",
	"Guillermo",
	"Diego",
	"Andrea",
	"Hashouang",
	"Bing",
	"Duda",
	"Marco",
	"Jordi",
	"Hugo"
]

static var DESCRIPTIONS: Array[String] = [
	"huele sorprendentemente bien",
	"jura que no ha roto el jarrón",
	"sabe lo que has hecho",
	"puede que no sea muy inteligente pero lo intenta",
	"niega todo incluso antes de ser acusado",
	"probablemente tiene un plan probablemente no",
	"sonríe demasiado para no estar tramando algo",
	"no fue su culpa casi seguro",
	"ha visto cosas no quiere hablar de ello",
	"se distrae con facilidad especialmente con comida",
	"promete que esta vez saldrá bien",
	"dice que sabe cocinar nadie ha sobrevivido para confirmarlo",
	"aplaude cuando el microondas termina",
	"siempre aparece cuando hay snacks",
	"no entiende del todo qué está pasando pero participa igual",
	"respira fuerte cuando piensa",
	"tiene energía caótica controlada más o menos",
	"dice 'confía en mí' demasiado a menudo",
	"podría ser un npc importante o no",
]


static func get_random_profile() -> String:
	var name: String = NAMES.pick_random()
	var description: String = DESCRIPTIONS.pick_random()
	return name + " " + description
