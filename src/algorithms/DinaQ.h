#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_DINAQ_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_DINAQ_H

#include <vector>
#include <deque>
#include <random>
#include <limits>
#include <tuple>
#include <algorithm>
#include <iostream>

template<typename TEnv>
class DinaQ {
public:
    DinaQ(std::size_t num_states, std::size_t num_actions, float learning_rate, float gamma, float epsilon,
          std::size_t memory_capacity, std::size_t batch_size, std::size_t target_update_freq)
            : num_states_(num_states),
              num_actions_(num_actions),
              learning_rate_(learning_rate),
              gamma_(gamma),
              epsilon_(epsilon),
              memory_capacity_(memory_capacity),
              batch_size_(batch_size),
              target_update_freq_(target_update_freq),
              steps_(0) {
        // Initialiser les Q-tables pour les réseaux en ligne et cible
        online_q_table_.assign(num_states, std::vector<float>(num_actions, 0.0f));
        target_q_table_.assign(num_states, std::vector<float>(num_actions, 0.0f));
    }

    void train(TEnv& env, std::size_t num_episodes) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        for (std::size_t episode = 0; episode < num_episodes; ++episode) {
            env.reset();
            std::size_t state = env.state_id();
            float total_reward = 0.0f;

            while (!env.is_game_over()) {
                // Sélectionner une action (epsilon-greedy)
                std::size_t action = select_action(state, dist(rng));

                // Exécuter l'action et observer la transition
                float previous_score = env.score();
                env.step(action);
                float reward = env.score() - previous_score;
                std::size_t next_state = env.state_id();
                bool done = env.is_game_over();

                // Stocker la transition dans la mémoire
                store_transition(state, action, reward, next_state, done);

                // Mettre à jour l'état
                state = next_state;
                total_reward += reward;

                // Effectuer une mise à jour si le batch est prêt
                if (memory_.size() >= batch_size_) {
                    optimize_model();
                }

                // Mettre à jour le réseau cible périodiquement
                if (steps_ % target_update_freq_ == 0) {
                    update_target_table();
                }

                steps_++;
            }

            std::cout << "Episode: " << episode << ", Total Reward: " << total_reward << std::endl;
        }
    }

private:
    std::size_t num_states_, num_actions_;
    float learning_rate_, gamma_, epsilon_;
    std::size_t memory_capacity_, batch_size_, target_update_freq_;
    std::size_t steps_;

    // Q-tables
    std::vector<std::vector<float>> online_q_table_, target_q_table_;

    // Mémoire de replay
    std::deque<std::tuple<std::size_t, std::size_t, float, std::size_t, bool>> memory_;

    // Sélectionner une action (epsilon-greedy)
    std::size_t select_action(std::size_t state, float random_value) {
        if (random_value < epsilon_) {
            // Action aléatoire
            return std::uniform_int_distribution<std::size_t>(0, num_actions_ - 1)(std::mt19937{std::random_device{}()});
        } else {
            // Action avec valeur Q maximale
            return std::distance(online_q_table_[state].begin(),
                                  std::max_element(online_q_table_[state].begin(), online_q_table_[state].end()));
        }
    }

    // Stocker une transition dans la mémoire de replay
    void store_transition(std::size_t state, std::size_t action, float reward, std::size_t next_state, bool done) {
        if (memory_.size() >= memory_capacity_) {
            memory_.pop_front();
        }
        memory_.emplace_back(state, action, reward, next_state, done);
    }

    // Optimiser le modèle
    void optimize_model() {
        // Échantillonner un batch
        std::vector<std::tuple<std::size_t, std::size_t, float, std::size_t, bool>> batch(batch_size_);
        std::sample(memory_.begin(), memory_.end(), batch.begin(), batch_size_, std::mt19937{std::random_device{}()});

        for (const auto& [state, action, reward, next_state, done] : batch) {
            float target = reward;

            // Si l'état suivant n'est pas terminal, ajouter la valeur Q estimée
            if (!done) {
                float max_next_q = *std::max_element(target_q_table_[next_state].begin(), target_q_table_[next_state].end());
                target += gamma_ * max_next_q;
            }

            // Mettre à jour la Q-value dans le réseau en ligne
            online_q_table_[state][action] += learning_rate_ * (target - online_q_table_[state][action]);
        }
    }

    // Mettre à jour le réseau cible
    void update_target_table() {
        target_q_table_ = online_q_table_;
    }
};

#endif // INC_5A_RVJV_FULL_CPP_TEMPLATE_DINAQ_H
