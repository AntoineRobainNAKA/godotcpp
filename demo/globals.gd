extends Node

# Doit correspondre à l'ordre dans les fichier .cpp
# Un peu sale mais si on fait des structs custom pour les param côté cpp,
# il faut tout implémenter pour la conversion cpp -> gdscript
# can't be arsed ¯\_(ツ)_/¯
enum AlgorithmType { 
	NONE,
	POLICY_ITERATION,
	VALUE_ITERATION,
	Q_LEARNING,
	MONTE_CARLO_ES,
	ON_POLICY_FIRST_VISIT_MC
}

enum EnvironmentType {
	NONE,
	LINEWORLD,
	GRIDWORLD,
	RPS,
	SECRETENV0
}
