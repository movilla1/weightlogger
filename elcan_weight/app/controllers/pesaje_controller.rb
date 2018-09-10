class PesajeController < ApplicationController
  def new
  end

  def create
  end

  def create_from_rfid
    data = params[:data]
    tag_id = data[0]
    time_part = data[1, 4]
    weight = data[5, 12]
    device = data[13]
    usuario = Usuario.find_tag(tag_id: tag_id)
    return false if usuario.blank?
    tmp_camion = usuario.camion
    camion_id = tmp_camion.id
    Pesaje.create(camion_id: camion_id, created_at: time_part,
                  eje: device, peso: weight)
    render json: 'OK', status: 200
  end
end
