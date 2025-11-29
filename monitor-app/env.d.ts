/// <reference types="vite/client" />

interface ImportMetaEnv {
  readonly VITE_CRATEDB_HOST: string
  readonly VITE_CRATEDB_PORT: string
}

interface ImportMeta {
  readonly env: ImportMetaEnv
}
