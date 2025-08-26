#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <imgui.h>

#include "types.hpp"

#include "gl_tiles.cpp"
#include "util.hpp"

struct Payload
{
    enum class Kind : int
    {
        NONE,
        Love,
        Salt,
        Dream,
        Asparagus,
        Spinach,
        WillowBranches,
        SealedLetter,
        WatermelonSlices,
        Notebook,
        Sun,
        Moon,
        Earth,
        Heaven,
        Location,
        Logos,
        Kairos,
        Employment,
        Longing,
        COUNT
    };

    Kind kind;

    Payload(Kind kind)
    {
        this->kind = kind;
    }

    static const char *get_kind_string(Kind kind)
    {
        switch (kind)
        {
            case Kind::NONE: return "NONE";
            case Kind::Love: return "Love";
            case Kind::Salt: return "Salt";
            case Kind::Dream: return "Dream";
            case Kind::Asparagus: return "Asparagus";
            case Kind::Spinach: return "Spinach";
            case Kind::WillowBranches: return "Willow Branches";
            case Kind::SealedLetter: return "Sealed Letter";
            case Kind::WatermelonSlices: return "Watermelon Slices";
            case Kind::Notebook: return "Notebook";
            case Kind::Sun: return "Sun";
            case Kind::Moon: return "Moon";
            case Kind::Earth: return "Earth";
            case Kind::Heaven: return "Heaven";
            case Kind::Location: return "Location";
            case Kind::Logos: return "Logos";
            case Kind::Kairos: return "Kairos";
            case Kind::Employment: return "Employment";
            case Kind::Longing: return "Longing";
            default: return "UNKNOWN";
        }
    }

    const char *get_kind_string()
    {
        return get_kind_string(kind);
    }

    static Kind get_random_kind()
    {
        Kind kind = (Kind)(rand() % (int)Kind::COUNT);
        return kind;
    }
};

struct Node
{
    char name_buf[STR_BUF_SMALL];
    bool is_window_open = false;

    int payload_counts[(int)Payload::Kind::COUNT];

    Node(const char *name)
    {
        trace("payload size: %zu", sizeof(payload_counts));
        memset(payload_counts, 0, sizeof(payload_counts));
        strcpy(this->name_buf, name);
    }

    static const char *get_random_name()
    {
        const char *names[] =
        {
            "Alpha",
            "Beta",
            "Gamma",
            "Delta",
            "Epsilon",
            "Zeta"
        };
        int i = rand() % array_size(names);
        return names[i];
    }
};

struct Agent
{
    char name_buf[STR_BUF_SMALL];
    bool is_window_open = false;
    size_t node_a = 0;
    size_t node_b = 0;
    float progress = 0.0f;
    float progress_rate = 0.3f;
    bool travelling_from_b = false;
    Payload carried_payload{Payload::Kind::NONE};

    Agent(const char *name)
    {
        strcpy(this->name_buf, name);
        carried_payload = {Payload::get_random_kind()};
    }

    inline bool destinations_valid()
    {
        return node_a > 0 && node_b > 0 && node_a != node_b;
    }

    void finish_delivery(std::vector<Node> &nodes)
    {
        if (carried_payload.kind != Payload::Kind::NONE)
        {
            size_t node_index;
            if (!travelling_from_b) node_index = node_b;
            else node_index = node_a;
            nodes[node_index].payload_counts[(int)carried_payload.kind]++;
        }
        carried_payload = {Payload::get_random_kind()};
        travelling_from_b = !travelling_from_b;
        progress = 0.0f;
    }

    void update_progress(std::vector<Node> &nodes, float delta)
    {
        if (destinations_valid())
        {
            progress += delta * progress_rate;
        }
        if (progress > 1.0f)
        {
            finish_delivery(nodes);
        }
    }

    static const char *get_random_name()
    {
        const char *random_names[] =
        {
            "Humgef",
            "Haliser",
            "Kierty",
            "Giolist",
            "Leemper"
        };
        int i = rand() % array_size(random_names);
        return random_names[i];
    }
};

struct Game
{
    std::vector<Agent> agents;
    std::vector<Node> nodes;

    char agent_list_name_edit_buf[STR_BUF_SMALL];
    char node_list_name_edit_buf[STR_BUF_SMALL];

    void init()
    {
        srand(0);
        agents.push_back(Agent("NONE"));
        nodes.push_back(Node("NONE"));
        strcpy(agent_list_name_edit_buf, Agent::get_random_name());
        strcpy(node_list_name_edit_buf, Node::get_random_name());
    }

    void frame(float delta)
    {
        for (size_t i = 1; i < agents.size(); i++)
        {
            agents[i].update_progress(nodes, delta);
        }

        draw_agent_list_window();

        draw_agent_windows();

        draw_node_list_window();

        draw_node_windows();
    }

    void draw_agent_list_window()
    {
        ImGui::Begin("Agents");

        ImGui::InputText("Name", agent_list_name_edit_buf, sizeof(agent_list_name_edit_buf));
        ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            agents.push_back(Agent(agent_list_name_edit_buf));
            strcpy(agent_list_name_edit_buf, Agent::get_random_name());
        }

        char item_name_buf[STR_BUF_SMALL];
        for (size_t i = 1; i < agents.size(); i++)
        {
            ImGui::PushID(agents.data() + i);
            ImGui::Bullet();
            snprintf(item_name_buf, sizeof(item_name_buf), "Agent %s", agents[i].name_buf);
            if (ImGui::TextLink(item_name_buf))
            {
                agents[i].is_window_open = !agents[i].is_window_open;
                trace("%zu: window open = %d", i, agents[i].is_window_open);
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    void draw_agent_windows()
    {
        for (size_t i = 1; i < agents.size(); i++)
        {
            if (agents[i].is_window_open)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

                char window_name_buf[STR_BUF_SMALL];
                snprintf(window_name_buf, sizeof(window_name_buf), "Agent: %s###Agent%zu", agents[i].name_buf, i);

                if (ImGui::Begin(window_name_buf, &agents[i].is_window_open))
                {
                    ImGui::InputText("Name", agents[i].name_buf, sizeof(agents[i].name_buf));

                    if (ImGui::BeginCombo("Node A", nodes[agents[i].node_a].name_buf, 0))
                    {
                        for (size_t node_i = 0; node_i < nodes.size(); node_i++)
                        {
                            const bool is_selected = agents[i].node_a == node_i;
                            if (ImGui::Selectable(nodes[node_i].name_buf, is_selected))
                            {
                                agents[i].node_a = node_i;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::BeginCombo("Node B", nodes[agents[i].node_b].name_buf, 0))
                    {
                        for (size_t node_i = 0; node_i < nodes.size(); node_i++)
                        {
                            const bool is_selected = agents[i].node_b == node_i;
                            if (ImGui::Selectable(nodes[node_i].name_buf, is_selected))
                            {
                                agents[i].node_b = node_i;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (agents[i].destinations_valid())
                    {
                        ImGui::BulletText("Travel direction: %s", agents[i].travelling_from_b ? "B -> A" : "A -> B");
                        ImGui::BulletText("Carried payload: %s", agents[i].carried_payload.get_kind_string());
                        ImGui::BulletText("Progress: %.3f", agents[i].progress);
                    }

                    ImGui::End();
                }
            }
        }
    }

    void draw_node_list_window()
    {
        ImGui::Begin("Nodes");

        ImGui::InputText("Name", node_list_name_edit_buf, sizeof(node_list_name_edit_buf));
        ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            nodes.push_back(Node(node_list_name_edit_buf));
            strcpy(node_list_name_edit_buf, Node::get_random_name());
        }

        char item_name_buf[STR_BUF_SMALL];
        for (size_t i = 1; i < nodes.size(); i++)
        {
            ImGui::PushID(nodes.data() + i);
            ImGui::Bullet();
            snprintf(item_name_buf, sizeof(item_name_buf), "Node %s", nodes[i].name_buf);
            if (ImGui::TextLink(item_name_buf))
            {
                nodes[i].is_window_open = !nodes[i].is_window_open;
                trace("%zu: window open = %d", i, nodes[i].is_window_open);
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    void draw_node_windows()
    {
        for (size_t node_i = 1; node_i < nodes.size(); node_i++)
        {
            if (nodes[node_i].is_window_open)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

                char window_name_buf[STR_BUF_SMALL];
                snprintf(window_name_buf, sizeof(window_name_buf), "Node: %s###Node%zu", nodes[node_i].name_buf, node_i);

                if (ImGui::Begin(window_name_buf, &nodes[node_i].is_window_open))
                {
                    ImGui::InputText("Name", nodes[node_i].name_buf, sizeof(nodes[node_i].name_buf));

                    bool any_payloads = false;
                    for (int kind_i = 1; kind_i < (int)Payload::Kind::COUNT; kind_i++)
                    {
                        if (nodes[node_i].payload_counts[kind_i] > 0)
                        {
                            any_payloads = true;
                            break;
                        }
                    }

                    if (any_payloads)
                    {
                        ImGui::Text("Payloads:");
                        for (int kind_i = 1; kind_i < (int)Payload::Kind::COUNT; kind_i++)
                        {
                            if (nodes[node_i].payload_counts[kind_i] > 0)
                            {
                                ImGui::BulletText("%d x %s", nodes[node_i].payload_counts[kind_i], Payload::get_kind_string((Payload::Kind)kind_i));
                            }
                        }
                    }

                    ImGui::End();
                }
            }
        }
    }
};
