class CreateCamiones < ActiveRecord::Migration
  def change
    create_table :camiones do |t|
      t.string :patente
      t.date :fecha_compra
      t.float :capacidad

      t.timestamps null: false
    end
  end
end
