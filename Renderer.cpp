#include "Renderer.h"

Renderer::Renderer() :
    scale(1, 1)
{
}

Renderer::~Renderer()
{
    //dtor
}

void Renderer::render(sf::RenderTarget &target){

    for (std::size_t i = 0; i < frameitems.size(); i++){
        if (frameitems[i].texture == nullptr)continue;
        sf::Sprite sp((*(frameitems[i].texture)), frameitems[i].rect);
        sp.scale(scale.x, scale.y);
        sp.setPosition(frameitems[i].pos);
        target.draw(sp);
    }

    frameitems.clear();
}

void Renderer::addRenderItem(sf::Texture *texture, const sf::Vector2f &pos, sf::IntRect rect){
    if (rect.width+rect.height == 0){
        rect.width = texture->getSize().x;
        rect.height = texture->getSize().y;
    }
    frameitems.push_back({texture, {pos.x*scale.x, pos.y*scale.y}, rect});
}

void Renderer::setScale(const sf::Vector2f &scale){
    this->scale = scale;
}

const sf::Vector2f &Renderer::getScale() const{
    return scale;
}
