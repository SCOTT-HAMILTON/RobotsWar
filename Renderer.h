#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

struct RenderItem{
    sf::Texture* texture;
    sf::Vector2f pos;
    sf::IntRect rect;
};

class Renderer
{
public:
    Renderer();
    virtual ~Renderer();
    void render(sf::RenderTarget &target);
    void addRenderItem(sf::Texture *texture, const sf::Vector2f &pos, sf::IntRect rect = sf::IntRect(0, 0, 0, 0));
    void setScale(const sf::Vector2f &scale);
    const sf::Vector2f &getScale() const;

private:
    std::vector<RenderItem> frameitems;
    sf::Vector2f scale;
};

#endif // RENDERER_H
