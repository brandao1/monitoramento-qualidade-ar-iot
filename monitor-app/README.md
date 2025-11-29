# Monitor de Qualidade do Ar - IoT

Uma aplicação Vue.js para monitoramento em tempo real da qualidade do ar, baseada no projeto [smart-meter](https://github.com/AlissonWeiss/smart-meter.git).

## Funcionalidades

- **Visualização em tempo real** dos dados de sensores de qualidade do ar
- **Atualização automática** configurável (2s, 5s, 10s, 30s, 1min)
- **Cards interativos** para cada métrica com indicadores visuais de cores
- **Interface responsiva** para desktop e dispositivos móveis
- **Conexão configurável** ao CrateDB

## Sensores Suportados

| Sensor | Descrição | Unidade |
|--------|-----------|---------|
| Temperatura | Temperatura ambiente | °C |
| Umidade | Umidade relativa do ar | % |
| CO₂ | Dióxido de carbono | ppm |
| CO | Monóxido de carbono | ppm |
| Tolueno | Concentração de tolueno | ppm |
| Amônia | Concentração de amônia | ppm |
| Acetona | Concentração de acetona | ppm |
| Ozônio | Concentração de ozônio | ppm |
| NO₂ | Dióxido de nitrogênio | ppm |
| Cl₂ | Cloro | ppm |
| PM 1.0 | Material particulado 1.0µm | µg/m³ |
| PM 2.5 | Material particulado 2.5µm | µg/m³ |
| PM 10.0 | Material particulado 10µm | µg/m³ |

## Pré-requisitos

- Node.js 20.19.0+ ou 22.12.0+
- CrateDB rodando (via Docker Compose do projeto principal)

## Configuração

### Variáveis de Ambiente (Opcional)

Crie um arquivo `.env` na raiz do projeto:

```env
VITE_CRATEDB_HOST=localhost
VITE_CRATEDB_PORT=4200
```

Ou configure diretamente na interface da aplicação.

## Instalação e Execução

```sh
# Instalar dependências
npm install

# Executar em modo desenvolvimento
npm run dev

# Compilar para produção
npm run build

# Pré-visualizar build de produção
npm run preview
```

## Uso

1. Inicie o backend FIWARE com Docker Compose (do projeto principal)
2. Execute a aplicação: `npm run dev`
3. Acesse `http://localhost:5173`
4. Configure o host e porta do CrateDB (padrão: localhost:4200)
5. Clique em "Conectar" para carregar os dados
6. Configure a atualização automática se desejar

## Tecnologias

- Vue.js 3 (Composition API)
- TypeScript
- Vite
- CrateDB (via FIWARE stack)

## IDE Recomendada

[VS Code](https://code.visualstudio.com/) + [Vue (Official)](https://marketplace.visualstudio.com/items?itemName=Vue.volar)

## Scripts Disponíveis

```sh
npm run dev      # Desenvolvimento com hot-reload
npm run build    # Build para produção
npm run preview  # Preview do build de produção
npm run lint     # Lint com ESLint
npm run format   # Formatar código com Prettier
```
