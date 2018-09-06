class CreatePesajes < ActiveRecord::Migration
  def change
    create_table :pesajes do |t|
      t.float :peso
      t.references :camion, index: true, foreign_key: true
      t.integer :eje
      t.boolean :completo

      t.timestamps null: false
    end
  end
end
