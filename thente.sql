--
-- PostgreSQL database dump
--

-- Dumped from database version 14.18 (Ubuntu 14.18-0ubuntu0.22.04.1)
-- Dumped by pg_dump version 14.18 (Ubuntu 14.18-0ubuntu0.22.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: Ujsema; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA "Ujsema";


ALTER SCHEMA "Ujsema" OWNER TO postgres;

--
-- Name: admin; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA admin;


ALTER SCHEMA admin OWNER TO postgres;

--
-- Name: app; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA app;


ALTER SCHEMA app OWNER TO postgres;

--
-- Name: assistant; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA assistant;


ALTER SCHEMA assistant OWNER TO postgres;

--
-- Name: lobby; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA lobby;


ALTER SCHEMA lobby OWNER TO postgres;

--
-- Name: munkatars; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA munkatars;


ALTER SCHEMA munkatars OWNER TO postgres;

--
-- Name: rszotar; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA rszotar;


ALTER SCHEMA rszotar OWNER TO postgres;

--
-- Name: sysadmin; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA sysadmin;


ALTER SCHEMA sysadmin OWNER TO postgres;

--
-- Name: szotar; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA szotar;


ALTER SCHEMA szotar OWNER TO postgres;

--
-- Name: users; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA users;


ALTER SCHEMA users OWNER TO postgres;

--
-- Name: login(text, text); Type: FUNCTION; Schema: lobby; Owner: postgres
--

CREATE FUNCTION lobby.login(infelhasznalonev text, injelszohash text) RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
	DECLARE fid int; newtoken int; 
	BEGIN select id into fid from sysadmin.felhasznalo 
	where TRIM(TRAILING FROM felhasznalonev) ilike infelhasznalonev and
       		jelszohash like injelszohash limit 1;
		RAISE NOTICE 'INO';
	       	if fid is not null then
			RAISE NOTICE 'IN';
		       	insert into sysadmin.bearertoken as syb (token, felhasznaloid) values ((select coalesce(pg_catalog.max(token), 0) from sysadmin.bearertoken) + 1, fid) returning syb.token into newtoken; 
	return newtoken; 
else 
	RAISE NOTICE 'ON';
	return 0; end if; END; $$;


ALTER FUNCTION lobby.login(infelhasznalonev text, injelszohash text) OWNER TO postgres;

--
-- Name: login(character varying, text); Type: FUNCTION; Schema: lobby; Owner: postgres
--

CREATE FUNCTION lobby.login(infelhasznalonev character varying, injelszohash text) RETURNS integer
    LANGUAGE plpgsql
    AS $$ 
	DECLARE fid int; newtoken int; 
	BEGIN select id into fid from sysadmin.felhasznalo 
	where /*felhasznalonev ilike infelhasznalonev and*/
       		jelszohash like injelszohash limit 1;
		RAISE NOTICE 'INO';
	       	if fid is not null then
			RAISE NOTICE 'IN';
		       	insert into sysadmin.bearertoken as syb (token, felhasznaloid) values ((select coalesce(pg_catalog.max(token), 0) from sysadmin.bearertoken) + 1, fid) returning syb.token into newtoken; 
	return newtoken; 
else 
	RAISE NOTICE 'ON';
	return 0; end if; END; $$;


ALTER FUNCTION lobby.login(infelhasznalonev character varying, injelszohash text) OWNER TO postgres;

--
-- Name: createuninjschema(text); Type: FUNCTION; Schema: sysadmin; Owner: postgres
--

CREATE FUNCTION sysadmin.createuninjschema(schemaname text) RETURNS boolean
    LANGUAGE plpgsql SECURITY DEFINER
    AS $$
DECLARE
lehet boolean;
BEGIN
        lehet := NOT EXISTS(select 1 from pg_get_keywords() where word ilike schemaname) and 
		NOT EXISTS (select 1 from pg_catalog.pg_namespace where nspname ilike schemaname);
        if lehet then
                EXECUTE format('create schema %I', schemaname);
	end if; RETURN lehet;
END; $$;


ALTER FUNCTION sysadmin.createuninjschema(schemaname text) OWNER TO postgres;

--
-- Name: getaccesfullschemasfromgroups(bigint, character, text); Type: FUNCTION; Schema: sysadmin; Owner: postgres
--

CREATE FUNCTION sysadmin.getaccesfullschemasfromgroups(usertoken bigint, sep character, schemacharchain text) RETURNS boolean
    LANGUAGE plpgsql
    AS $$
 DECLARE
 groups int[];
 schemalist text[];
 schemaID int;
 benne boolean;
 target text;
 BEGIN
        benne := true;
         groups := sysadmin.getusergroups(sysadmin.getuserfromtoken(usertoken));
         schemalist := string_to_array(schemacharchain, sep);
         foreach target in array schemalist loop 
		EXIT WHEN NOT benne;
		Continue when target = '';
                 schemaID := (select oid from pg_catalog.pg_namespace where nspname ilike target);
                 benne := ((select count(*) from sysadmin.engedelyezettsema where schemaid = semaID and jogcsoportid = ANY(groups)) > 0);
         end loop;
         return benne;
 end; $$;


ALTER FUNCTION sysadmin.getaccesfullschemasfromgroups(usertoken bigint, sep character, schemacharchain text) OWNER TO postgres;

--
-- Name: getuserfromtoken(bigint); Type: FUNCTION; Schema: sysadmin; Owner: postgres
--

CREATE FUNCTION sysadmin.getuserfromtoken(usertoken bigint) RETURNS integer
    LANGUAGE plpgsql
    AS $$ BEGIN return (select felhasznaloID FROM sysadmin.bearertoken where token=usertoken); END; $$;


ALTER FUNCTION sysadmin.getuserfromtoken(usertoken bigint) OWNER TO postgres;

--
-- Name: getusergroups(integer); Type: FUNCTION; Schema: sysadmin; Owner: postgres
--

CREATE FUNCTION sysadmin.getusergroups(userid integer) RETURNS integer[]
    LANGUAGE plpgsql
    AS $$ 
BEGIN
	return ARRAY((select jogcsoportID from sysadmin.jogcsoporthozzarendeles where felhasznaloID=userID)); END;
$$;


ALTER FUNCTION sysadmin.getusergroups(userid integer) OWNER TO postgres;

--
-- Name: isnotinschemalist(character, text); Type: FUNCTION; Schema: sysadmin; Owner: postgres
--

CREATE FUNCTION sysadmin.isnotinschemalist(sep character, columncharchain text) RETURNS boolean
    LANGUAGE plpgsql
    AS $$
	DECLARE
		columnlist text[];
		benne boolean;
		target text;
	BEGIN
		benne := true;
	       	columnlist := string_to_array(columncharchain, sep);
	       	foreach target in array columnlist loop EXIT WHEN NOT benne;
			Continue when target = '';
			benne := (select count(*) from pg_catalog.pg_namespace where nspname ilike target);
		end loop; 
	return benne;
	end;
$$;


ALTER FUNCTION sysadmin.isnotinschemalist(sep character, columncharchain text) OWNER TO postgres;

--
-- Name: validate_semaid(); Type: FUNCTION; Schema: sysadmin; Owner: postgres
--

CREATE FUNCTION sysadmin.validate_semaid() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    -- Ellenőrizzük, hogy létezik-e ilyen séma OID
    IF NOT EXISTS (
        SELECT 1 FROM pg_catalog.pg_namespace WHERE oid = NEW.semaID
    ) THEN
        RAISE EXCEPTION 'Nem létező séma OID: %', NEW.semaID;
    END IF;

    RETURN NEW;
END;
$$;


ALTER FUNCTION sysadmin.validate_semaid() OWNER TO postgres;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: alanyvizsgalat; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.alanyvizsgalat (
    id integer NOT NULL,
    vizsgalatelojegyzesid integer NOT NULL,
    alanyid integer,
    vizsgalatidopont date,
    megjegyzes text
);


ALTER TABLE admin.alanyvizsgalat OWNER TO postgres;

--
-- Name: AlanyVizsgalat_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.alanyvizsgalat ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."AlanyVizsgalat_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: alany; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.alany (
    id integer NOT NULL,
    kodnev character(30) NOT NULL,
    nev character varying(100) NOT NULL,
    nem bit(1),
    szuletes date,
    fajid integer
);


ALTER TABLE admin.alany OWNER TO postgres;

--
-- Name: Alany_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.alany ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Alany_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: alkotoresztipus; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.alkotoresztipus (
    id integer NOT NULL,
    elnevezes character varying(100) NOT NULL
);


ALTER TABLE admin.alkotoresztipus OWNER TO postgres;

--
-- Name: AlkotoreszTipus_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.alkotoresztipus ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."AlkotoreszTipus_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: alkotoresz; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.alkotoresz (
    id integer NOT NULL,
    alkotoresztipusid integer,
    alkotoreszidazonbelul integer,
    elnevezes character varying(30) NOT NULL
);


ALTER TABLE admin.alkotoresz OWNER TO postgres;

--
-- Name: Alkotoresz_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.alkotoresz ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Alkotoresz_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: anyag; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.anyag (
    id integer NOT NULL,
    nev character varying(100) NOT NULL,
    mennyiseg double precision,
    mertekegysegid integer
);


ALTER TABLE admin.anyag OWNER TO postgres;

--
-- Name: Anyag_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.anyag ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Anyag_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: ceg; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.ceg (
    id integer NOT NULL,
    nev character varying(200) NOT NULL
);


ALTER TABLE admin.ceg OWNER TO postgres;

--
-- Name: Ceg_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.ceg ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Ceg_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: epulethelyseg; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.epulethelyseg (
    id integer NOT NULL,
    elnevezes character varying(30) NOT NULL
);


ALTER TABLE admin.epulethelyseg OWNER TO postgres;

--
-- Name: EpuletHelyseg_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.epulethelyseg ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."EpuletHelyseg_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: etetes; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.etetes (
    id integer NOT NULL,
    alanyid integer NOT NULL,
    datum date NOT NULL,
    munkatarsidseged integer
);


ALTER TABLE admin.etetes OWNER TO postgres;

--
-- Name: Etetes_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.etetes ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Etetes_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: faj; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.faj (
    id integer NOT NULL,
    elnevezes character varying(100) NOT NULL
);


ALTER TABLE admin.faj OWNER TO postgres;

--
-- Name: Faj_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.faj ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Faj_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: klcshozzarendelesvegrehajtas; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.klcshozzarendelesvegrehajtas (
    id integer NOT NULL,
    kiserletvegrehajtasid integer NOT NULL,
    klcshozzarendelesid integer NOT NULL,
    kezd date NOT NULL,
    vege date,
    munkatarsidvezeto integer
);


ALTER TABLE admin.klcshozzarendelesvegrehajtas OWNER TO postgres;

--
-- Name: KLCsHozzarendelesVegrehajtas_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.klcshozzarendelesvegrehajtas ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KLCsHozzarendelesVegrehajtas_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: klcshozzarendeles; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.klcshozzarendeles (
    id integer NOT NULL,
    fazis integer NOT NULL,
    kiserletid integer NOT NULL,
    kiserletlepescsoportid integer NOT NULL,
    csoportszam integer,
    elnevezes character varying(100) NOT NULL,
    leiras text
);


ALTER TABLE admin.klcshozzarendeles OWNER TO postgres;

--
-- Name: KLCsHozzarendeles_fazis_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.klcshozzarendeles ALTER COLUMN fazis ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KLCsHozzarendeles_fazis_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: KLCsHozzarendeles_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.klcshozzarendeles ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KLCsHozzarendeles_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserletlepescsoport; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepescsoport (
    id integer NOT NULL,
    elnevezes character varying(100) NOT NULL,
    leiras text
);


ALTER TABLE admin.kiserletlepescsoport OWNER TO postgres;

--
-- Name: KiserletLepesCsoport_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletlepescsoport ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletLepesCsoport_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserletlepesvegrehajtasesemeny; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesvegrehajtasesemeny (
    hanyadik integer NOT NULL,
    kiserletlepesvegrehajtasid integer NOT NULL,
    mikor date,
    gyorsleiras character varying(200),
    elemzes text
);


ALTER TABLE admin.kiserletlepesvegrehajtasesemeny OWNER TO postgres;

--
-- Name: KiserletLepesVegrehajtasEsemeny_hanyadik_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletlepesvegrehajtasesemeny ALTER COLUMN hanyadik ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletLepesVegrehajtasEsemeny_hanyadik_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserletlepesvegrehajtas; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesvegrehajtas (
    id integer NOT NULL,
    probalkozas integer NOT NULL,
    klcshozzarendelesvegrehajtasid integer NOT NULL,
    kiserletlepesid integer NOT NULL,
    alanyid integer NOT NULL,
    mikor date,
    munkatarsid integer,
    sikeres bit(1),
    leiras text
);


ALTER TABLE admin.kiserletlepesvegrehajtas OWNER TO postgres;

--
-- Name: KiserletLepesVegrehajtas_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletlepesvegrehajtas ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletLepesVegrehajtas_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: KiserletLepesVegrehajtas_probalkozas_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletlepesvegrehajtas ALTER COLUMN probalkozas ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletLepesVegrehajtas_probalkozas_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserletlepes; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepes (
    id integer NOT NULL,
    kiserletlepescsoportid integer NOT NULL,
    elnevezes character varying(100) NOT NULL,
    epulethelysegid integer,
    leiras text
);


ALTER TABLE admin.kiserletlepes OWNER TO postgres;

--
-- Name: KiserletLepes_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletlepes ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletLepes_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserletvegrehajtas; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletvegrehajtas (
    id integer NOT NULL,
    kiserletid integer NOT NULL,
    mikor date NOT NULL,
    munkatarsid integer
);


ALTER TABLE admin.kiserletvegrehajtas OWNER TO postgres;

--
-- Name: KiserletVegrehajtas_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletvegrehajtas ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletVegrehajtas_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserlet; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserlet (
    id integer NOT NULL,
    elnevezes integer NOT NULL,
    leiras text
);


ALTER TABLE admin.kiserlet OWNER TO postgres;

--
-- Name: Kiserlet_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserlet ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Kiserlet_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiserletlepesfeltetelesugras; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesfeltetelesugras (
    id integer NOT NULL,
    kiserletlepesid integer NOT NULL,
    kiserletlepesidcel integer NOT NULL,
    rekurzivnakszant bit(1)
);


ALTER TABLE admin.kiserletlepesfeltetelesugras OWNER TO postgres;

--
-- Name: KiserletlepesFeltetelesUgras_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.kiserletlepesfeltetelesugras ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."KiserletlepesFeltetelesUgras_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: munkatars; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.munkatars (
    id integer NOT NULL,
    szerepkorid integer NOT NULL
);


ALTER TABLE admin.munkatars OWNER TO postgres;

--
-- Name: Munkatars_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.munkatars ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Munkatars_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: oktatasiintezmeny; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.oktatasiintezmeny (
    id integer NOT NULL,
    nev character varying(200) NOT NULL
);


ALTER TABLE admin.oktatasiintezmeny OWNER TO postgres;

--
-- Name: OktatasiIntezmeny_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.oktatasiintezmeny ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."OktatasiIntezmeny_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: szak; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.szak (
    id integer NOT NULL,
    oktatasiintezmenyid integer,
    szakmegnevezes character varying(200) NOT NULL
);


ALTER TABLE admin.szak OWNER TO postgres;

--
-- Name: Szak_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.szak ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Szak_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: szemely; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.szemely (
    id integer NOT NULL,
    szemelyiszam character(6),
    nev character varying(100) NOT NULL,
    anyjaneve character varying(100) NOT NULL,
    szuletesiido date NOT NULL
);


ALTER TABLE admin.szemely OWNER TO postgres;

--
-- Name: Személy_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.szemely ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Személy_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: szerepkor; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.szerepkor (
    id integer NOT NULL,
    elvevezes character varying(100) NOT NULL
);


ALTER TABLE admin.szerepkor OWNER TO postgres;

--
-- Name: Szerepkor_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.szerepkor ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Szerepkor_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tulajdonsag; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.tulajdonsag (
    id integer NOT NULL,
    elnevezes character varying(30) NOT NULL,
    mertekegysegid integer
);


ALTER TABLE admin.tulajdonsag OWNER TO postgres;

--
-- Name: Tualjdonsag_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.tulajdonsag ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Tualjdonsag_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tunet; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.tunet (
    id integer NOT NULL,
    elnevezes character varying(100) NOT NULL,
    leiras text
);


ALTER TABLE admin.tunet OWNER TO postgres;

--
-- Name: Tunet_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.tunet ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Tunet_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: valaszthatoalanytipus; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.valaszthatoalanytipus (
    id integer NOT NULL,
    kiserletlepesid integer NOT NULL,
    vizsgalatelojegyzesid integer NOT NULL,
    mennyi integer,
    csoportszam integer
);


ALTER TABLE admin.valaszthatoalanytipus OWNER TO postgres;

--
-- Name: ValaszthatoAlanyTipus_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.valaszthatoalanytipus ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."ValaszthatoAlanyTipus_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: valaszthato; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.valaszthato (
    id integer NOT NULL,
    elnevezes character varying(30) NOT NULL
);


ALTER TABLE admin.valaszthato OWNER TO postgres;

--
-- Name: Valaszthato_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.valaszthato ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."Valaszthato_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: vizsgalatelojegyzes; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.vizsgalatelojegyzes (
    id integer NOT NULL,
    vizsgalattipusid integer,
    mintfeltetelletezik bit(1)
);


ALTER TABLE admin.vizsgalatelojegyzes OWNER TO postgres;

--
-- Name: VizsgalatElojegyzes_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.vizsgalatelojegyzes ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."VizsgalatElojegyzes_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: vizsgalattipus; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.vizsgalattipus (
    id integer NOT NULL,
    elnevezes character varying(30) NOT NULL
);


ALTER TABLE admin.vizsgalattipus OWNER TO postgres;

--
-- Name: VizsgalatTipus_id_seq; Type: SEQUENCE; Schema: admin; Owner: postgres
--

ALTER TABLE admin.vizsgalattipus ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME admin."VizsgalatTipus_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: alanytulajdonsag; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.alanytulajdonsag (
    vizsgalatid integer NOT NULL,
    tulajdonsagid integer NOT NULL,
    mennyi double precision,
    kevesebb bit(1),
    egyenlo bit(1)
);


ALTER TABLE admin.alanytulajdonsag OWNER TO postgres;

--
-- Name: alkotoresztulajdonsag; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.alkotoresztulajdonsag (
    vizsgalatid integer NOT NULL,
    alkotoreszid integer NOT NULL,
    tulajdonsagid integer NOT NULL,
    mennyi double precision,
    kevesebb bit(1),
    egyenlo bit(1),
    leiras text
);


ALTER TABLE admin.alkotoresztulajdonsag OWNER TO postgres;

--
-- Name: atjaras; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.atjaras (
    epulethelysegid integer NOT NULL,
    epulethelysegidcel integer NOT NULL
);


ALTER TABLE admin.atjaras OWNER TO postgres;

--
-- Name: epulethelysegferohely; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.epulethelysegferohely (
    epulethelysegid integer NOT NULL,
    kiserletcsoportid integer NOT NULL,
    mennyien integer
);


ALTER TABLE admin.epulethelysegferohely OWNER TO postgres;

--
-- Name: erintettalkotoresz; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.erintettalkotoresz (
    alkotoreszid integer NOT NULL,
    tunetid integer NOT NULL
);


ALTER TABLE admin.erintettalkotoresz OWNER TO postgres;

--
-- Name: etelintolarencia; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.etelintolarencia (
    etetesid integer,
    anyagid integer
);


ALTER TABLE admin.etelintolarencia OWNER TO postgres;

--
-- Name: fajalkotoresze; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.fajalkotoresze (
    fajid integer NOT NULL,
    alkotoreszid integer NOT NULL
);


ALTER TABLE admin.fajalkotoresze OWNER TO postgres;

--
-- Name: halottalany; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.halottalany (
    alanyid integer NOT NULL,
    halalidopontja date,
    leiras text
);


ALTER TABLE admin.halottalany OWNER TO postgres;

--
-- Name: kiserletlepesfeltetelesugrasfeltetel; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesfeltetelesugrasfeltetel (
    kiserletlepesfeltetelesugrasid integer NOT NULL,
    vizsgalatelojegyzesid integer NOT NULL
);


ALTER TABLE admin.kiserletlepesfeltetelesugrasfeltetel OWNER TO postgres;

--
-- Name: kiserletlepessorrend; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepessorrend (
    kiserletlepesid integer,
    hanyadik integer,
    kiserletlepesidszuloelem integer,
    csoportszam integer
);


ALTER TABLE admin.kiserletlepessorrend OWNER TO postgres;

--
-- Name: kiserletlepesvegrehajtasetetes; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesvegrehajtasetetes (
    kiserletlepesvegrehajtasid integer NOT NULL,
    etetesid integer NOT NULL
);


ALTER TABLE admin.kiserletlepesvegrehajtasetetes OWNER TO postgres;

--
-- Name: kiserletlepesvegrehajtasvalasztottalany; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesvegrehajtasvalasztottalany (
    kiserletlepesvegrehajtasid integer NOT NULL,
    alanyid integer NOT NULL
);


ALTER TABLE admin.kiserletlepesvegrehajtasvalasztottalany OWNER TO postgres;

--
-- Name: kiserletlepesvegrehajtasvalasztottlehetoseg; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesvegrehajtasvalasztottlehetoseg (
    kiserletlepesvegrehajtasid integer NOT NULL,
    valaszthatoid integer NOT NULL
);


ALTER TABLE admin.kiserletlepesvegrehajtasvalasztottlehetoseg OWNER TO postgres;

--
-- Name: kiserletlepesvegrehajtasvizsgalat; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletlepesvegrehajtasvizsgalat (
    kiserletlepesvegrehajtasid integer NOT NULL,
    alanyvizsgalatid integer NOT NULL
);


ALTER TABLE admin.kiserletlepesvegrehajtasvizsgalat OWNER TO postgres;

--
-- Name: kiserletresztvevo; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.kiserletresztvevo (
    klcshozzarendelesvegrehajtasid integer NOT NULL,
    alanyid integer NOT NULL,
    csoportszam integer,
    bevonasdatuma date,
    kivonasdatuma date,
    alanyvizsgalata text
);


ALTER TABLE admin.kiserletresztvevo OWNER TO postgres;

--
-- Name: lepesheztartozovizsgalat; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.lepesheztartozovizsgalat (
    kiserletlepesid integer NOT NULL,
    vizsgalatelojegyzesid integer NOT NULL
);


ALTER TABLE admin.lepesheztartozovizsgalat OWNER TO postgres;

--
-- Name: megetetendoetelek; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.megetetendoetelek (
    kiserletlepesid integer NOT NULL,
    anyagid integer NOT NULL,
    mennyiseg double precision
);


ALTER TABLE admin.megetetendoetelek OWNER TO postgres;

--
-- Name: megetetettetelek; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.megetetettetelek (
    etetesid integer NOT NULL,
    anyagid integer NOT NULL,
    mennyiseg double precision,
    elteroidopont date
);


ALTER TABLE admin.megetetettetelek OWNER TO postgres;

--
-- Name: oklevel; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.oklevel (
    munkatarsid integer NOT NULL,
    oktatasiintezmenyid integer NOT NULL,
    szakid integer NOT NULL,
    kezdeseve integer NOT NULL,
    vegzeseve integer,
    ertekeles integer,
    befejezetlen bit(1)
);


ALTER TABLE admin.oklevel OWNER TO postgres;

--
-- Name: recept; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.recept (
    anyagid integer NOT NULL,
    anyagidosszetevo integer NOT NULL,
    variacionevid integer NOT NULL,
    mennyiseg double precision
);


ALTER TABLE admin.recept OWNER TO postgres;

--
-- Name: receptvariacionev; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.receptvariacionev (
    id integer NOT NULL,
    nev character varying(30)
);


ALTER TABLE admin.receptvariacionev OWNER TO postgres;

--
-- Name: tapasztalat; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.tapasztalat (
    munkatarsid integer NOT NULL,
    cegid integer NOT NULL,
    munkaviszonykezdete date NOT NULL,
    munkakor character varying(200),
    leiras text,
    munkaviszonyvege date
);


ALTER TABLE admin.tapasztalat OWNER TO postgres;

--
-- Name: tartalmaz; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.tartalmaz (
    vizsgalatid integer NOT NULL,
    alkotoreszid integer NOT NULL,
    anyagid integer NOT NULL,
    mennyiseg double precision,
    kevesebb bit(1),
    egyenlo bit(1)
);


ALTER TABLE admin.tartalmaz OWNER TO postgres;

--
-- Name: tunetallapot; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.tunetallapot (
    vizsgalatid integer NOT NULL,
    tunetid integer NOT NULL,
    sulyossagmerteke integer,
    kevesebb bit(1),
    egyenlo bit(1),
    leiras text
);


ALTER TABLE admin.tunetallapot OWNER TO postgres;

--
-- Name: valasztasilehetoseg; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.valasztasilehetoseg (
    kiserletlepesid integer NOT NULL,
    valaszthatoid integer NOT NULL,
    csoportszam integer
);


ALTER TABLE admin.valasztasilehetoseg OWNER TO postgres;

--
-- Name: valaszthatoalany; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.valaszthatoalany (
    klcshozzarendelesvegrehajtasid integer NOT NULL,
    alanyid integer NOT NULL,
    valaszthatoalanytipusid integer
);


ALTER TABLE admin.valaszthatoalany OWNER TO postgres;

--
-- Name: valaszthatoanyag; Type: TABLE; Schema: admin; Owner: postgres
--

CREATE TABLE admin.valaszthatoanyag (
    kiserletlepesid integer NOT NULL,
    anyagid integer NOT NULL,
    csoportszam integer
);


ALTER TABLE admin.valaszthatoanyag OWNER TO postgres;

--
-- Name: felhasznalo; Type: TABLE; Schema: sysadmin; Owner: postgres
--

CREATE TABLE sysadmin.felhasznalo (
    id integer NOT NULL,
    felhasznalonev character(30),
    email character varying(60),
    jelszohash text
);


ALTER TABLE sysadmin.felhasznalo OWNER TO postgres;

--
-- Name: felhasznaloview; Type: VIEW; Schema: lobby; Owner: postgres
--

CREATE VIEW lobby.felhasznaloview AS
 SELECT felhasznalo.id,
    felhasznalo.felhasznalonev,
    felhasznalo.email,
    felhasznalo.jelszohash
   FROM sysadmin.felhasznalo;


ALTER TABLE lobby.felhasznaloview OWNER TO postgres;

--
-- Name: Felhasznalo_id_seq; Type: SEQUENCE; Schema: sysadmin; Owner: postgres
--

ALTER TABLE sysadmin.felhasznalo ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME sysadmin."Felhasznalo_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: jogcsoport; Type: TABLE; Schema: sysadmin; Owner: postgres
--

CREATE TABLE sysadmin.jogcsoport (
    id integer NOT NULL,
    nev character varying(30)
);


ALTER TABLE sysadmin.jogcsoport OWNER TO postgres;

--
-- Name: JogCsoport_id_seq; Type: SEQUENCE; Schema: sysadmin; Owner: postgres
--

ALTER TABLE sysadmin.jogcsoport ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME sysadmin."JogCsoport_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: bearertoken; Type: TABLE; Schema: sysadmin; Owner: postgres
--

CREATE TABLE sysadmin.bearertoken (
    token bigint NOT NULL,
    felhasznaloid integer
);


ALTER TABLE sysadmin.bearertoken OWNER TO postgres;

--
-- Name: columnsview; Type: VIEW; Schema: sysadmin; Owner: postgres
--

CREATE VIEW sysadmin.columnsview AS
 SELECT columns.table_schema,
    columns.table_name,
    columns.column_name,
    columns.data_type,
    columns.character_maximum_length
   FROM information_schema.columns
  ORDER BY columns.table_schema, columns.table_name, columns.column_name;


ALTER TABLE sysadmin.columnsview OWNER TO postgres;

--
-- Name: engedelyezettsema; Type: TABLE; Schema: sysadmin; Owner: postgres
--

CREATE TABLE sysadmin.engedelyezettsema (
    jogcsoportid integer NOT NULL,
    semaid integer NOT NULL,
    hozzafereskategoriaid integer
);


ALTER TABLE sysadmin.engedelyezettsema OWNER TO postgres;

--
-- Name: hozzafereskategoria; Type: TABLE; Schema: sysadmin; Owner: postgres
--

CREATE TABLE sysadmin.hozzafereskategoria (
    id integer NOT NULL,
    nev integer
);


ALTER TABLE sysadmin.hozzafereskategoria OWNER TO postgres;

--
-- Name: jogcsoporthozzarendeles; Type: TABLE; Schema: sysadmin; Owner: postgres
--

CREATE TABLE sysadmin.jogcsoporthozzarendeles (
    jogcsoportid integer NOT NULL,
    felhasznaloid integer NOT NULL
);


ALTER TABLE sysadmin.jogcsoporthozzarendeles OWNER TO postgres;

--
-- Name: jogengedelyview; Type: VIEW; Schema: sysadmin; Owner: postgres
--

CREATE VIEW sysadmin.jogengedelyview AS
 SELECT j.nev,
    n.nspname
   FROM ((sysadmin.engedelyezettsema e
     JOIN sysadmin.jogcsoport j ON ((j.id = e.jogcsoportid)))
     JOIN pg_namespace n ON ((n.oid = (e.semaid)::oid)));


ALTER TABLE sysadmin.jogengedelyview OWNER TO postgres;

--
-- Name: mertekegyseg; Type: TABLE; Schema: szotar; Owner: postgres
--

CREATE TABLE szotar.mertekegyseg (
    id integer NOT NULL,
    elnevezes character varying(100) NOT NULL,
    rovidites character varying(6) NOT NULL
);


ALTER TABLE szotar.mertekegyseg OWNER TO postgres;

--
-- Name: Mertekegyseg_id_seq; Type: SEQUENCE; Schema: szotar; Owner: postgres
--

ALTER TABLE szotar.mertekegyseg ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME szotar."Mertekegyseg_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: personview; Type: VIEW; Schema: users; Owner: postgres
--

CREATE VIEW users.personview AS
 SELECT alany.id,
    alany.kodnev,
    alany.nev,
    alany.nem,
    alany.szuletes,
    alany.fajid
   FROM admin.alany
  WHERE (alany.id = sysadmin.getuserfromtoken((current_setting('app.token'::text))::bigint))
  WITH CASCADED CHECK OPTION;


ALTER TABLE users.personview OWNER TO postgres;

--
-- Name: profileview; Type: VIEW; Schema: users; Owner: postgres
--

CREATE VIEW users.profileview AS
 SELECT felhasznalo.id,
    felhasznalo.felhasznalonev,
    felhasznalo.email,
    felhasznalo.jelszohash
   FROM sysadmin.felhasznalo
  WHERE (felhasznalo.id = sysadmin.getuserfromtoken((current_setting('app.token'::text))::bigint));


ALTER TABLE users.profileview OWNER TO postgres;

--
-- Name: alanytulajdonsag AlanyTulajdonsag_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanytulajdonsag
    ADD CONSTRAINT "AlanyTulajdonsag_pkey" PRIMARY KEY (vizsgalatid, tulajdonsagid);


--
-- Name: alanyvizsgalat AlanyVizsgalat_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanyvizsgalat
    ADD CONSTRAINT "AlanyVizsgalat_id_key" UNIQUE (id);


--
-- Name: alanyvizsgalat AlanyVizsgalat_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanyvizsgalat
    ADD CONSTRAINT "AlanyVizsgalat_pkey" PRIMARY KEY (vizsgalatelojegyzesid);


--
-- Name: alany Alany_kodnev_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alany
    ADD CONSTRAINT "Alany_kodnev_key" UNIQUE (kodnev);


--
-- Name: alany Alany_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alany
    ADD CONSTRAINT "Alany_pkey" PRIMARY KEY (id);


--
-- Name: alkotoresztipus AlkotoreszTipus_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresztipus
    ADD CONSTRAINT "AlkotoreszTipus_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: alkotoresztipus AlkotoreszTipus_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresztipus
    ADD CONSTRAINT "AlkotoreszTipus_pkey" PRIMARY KEY (id);


--
-- Name: alkotoresztulajdonsag AlkotoreszTulajdonsag_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresztulajdonsag
    ADD CONSTRAINT "AlkotoreszTulajdonsag_pkey" PRIMARY KEY (vizsgalatid, alkotoreszid, tulajdonsagid);


--
-- Name: alkotoresz Alkotoresz_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresz
    ADD CONSTRAINT "Alkotoresz_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: alkotoresz Alkotoresz_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresz
    ADD CONSTRAINT "Alkotoresz_pkey" PRIMARY KEY (id);


--
-- Name: anyag Anyag_nev_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.anyag
    ADD CONSTRAINT "Anyag_nev_key" UNIQUE (nev);


--
-- Name: anyag Anyag_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.anyag
    ADD CONSTRAINT "Anyag_pkey" PRIMARY KEY (id);


--
-- Name: atjaras Atjaras_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.atjaras
    ADD CONSTRAINT "Atjaras_pkey" PRIMARY KEY (epulethelysegid, epulethelysegidcel);


--
-- Name: ceg Ceg_nev_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.ceg
    ADD CONSTRAINT "Ceg_nev_key" UNIQUE (nev);


--
-- Name: ceg Ceg_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.ceg
    ADD CONSTRAINT "Ceg_pkey" PRIMARY KEY (id);


--
-- Name: epulethelysegferohely EpuletHelysegFerohely_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.epulethelysegferohely
    ADD CONSTRAINT "EpuletHelysegFerohely_pkey" PRIMARY KEY (epulethelysegid, kiserletcsoportid);


--
-- Name: epulethelyseg EpuletHelyseg_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.epulethelyseg
    ADD CONSTRAINT "EpuletHelyseg_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: epulethelyseg EpuletHelyseg_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.epulethelyseg
    ADD CONSTRAINT "EpuletHelyseg_pkey" PRIMARY KEY (id);


--
-- Name: erintettalkotoresz ErintettAlkotoresz_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.erintettalkotoresz
    ADD CONSTRAINT "ErintettAlkotoresz_pkey" PRIMARY KEY (alkotoreszid, tunetid);


--
-- Name: etetes Etetes_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.etetes
    ADD CONSTRAINT "Etetes_id_key" UNIQUE (id);


--
-- Name: etetes Etetes_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.etetes
    ADD CONSTRAINT "Etetes_pkey" PRIMARY KEY (alanyid, datum);


--
-- Name: fajalkotoresze FajAlkotoresze_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.fajalkotoresze
    ADD CONSTRAINT "FajAlkotoresze_pkey" PRIMARY KEY (fajid, alkotoreszid);


--
-- Name: faj Faj_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.faj
    ADD CONSTRAINT "Faj_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: faj Faj_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.faj
    ADD CONSTRAINT "Faj_pkey" PRIMARY KEY (id);


--
-- Name: halottalany HalottAlany_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.halottalany
    ADD CONSTRAINT "HalottAlany_pkey" PRIMARY KEY (alanyid);


--
-- Name: klcshozzarendelesvegrehajtas KLCsHozzarendelesVegrehajtas_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendelesvegrehajtas
    ADD CONSTRAINT "KLCsHozzarendelesVegrehajtas_id_key" UNIQUE (id);


--
-- Name: klcshozzarendelesvegrehajtas KLCsHozzarendelesVegrehajtas_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendelesvegrehajtas
    ADD CONSTRAINT "KLCsHozzarendelesVegrehajtas_pkey" PRIMARY KEY (kiserletvegrehajtasid, klcshozzarendelesid, kezd);


--
-- Name: klcshozzarendeles KLCsHozzarendeles_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendeles
    ADD CONSTRAINT "KLCsHozzarendeles_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: klcshozzarendeles KLCsHozzarendeles_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendeles
    ADD CONSTRAINT "KLCsHozzarendeles_id_key" UNIQUE (id);


--
-- Name: klcshozzarendeles KLCsHozzarendeles_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendeles
    ADD CONSTRAINT "KLCsHozzarendeles_pkey" PRIMARY KEY (fazis, kiserletid, kiserletlepescsoportid);


--
-- Name: kiserletlepescsoport KiserletLepesCsoport_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepescsoport
    ADD CONSTRAINT "KiserletLepesCsoport_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: kiserletlepescsoport KiserletLepesCsoport_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepescsoport
    ADD CONSTRAINT "KiserletLepesCsoport_id_key" UNIQUE (id);


--
-- Name: kiserletlepesfeltetelesugrasfeltetel KiserletLepesFeltetelesUgrasFeltetel_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugrasfeltetel
    ADD CONSTRAINT "KiserletLepesFeltetelesUgrasFeltetel_pkey" PRIMARY KEY (kiserletlepesfeltetelesugrasid, vizsgalatelojegyzesid);


--
-- Name: kiserletlepesvegrehajtasetetes KiserletLepesVegreHajtasEtetes_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasetetes
    ADD CONSTRAINT "KiserletLepesVegreHajtasEtetes_pkey" PRIMARY KEY (kiserletlepesvegrehajtasid, etetesid);


--
-- Name: kiserletlepesvegrehajtasvalasztottalany KiserletLepesVegreHajtasValasztottAlany_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvalasztottalany
    ADD CONSTRAINT "KiserletLepesVegreHajtasValasztottAlany_pkey" PRIMARY KEY (kiserletlepesvegrehajtasid, alanyid);


--
-- Name: kiserletlepesvegrehajtasvalasztottlehetoseg KiserletLepesVegreHajtasValasztottLehetoseg_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvalasztottlehetoseg
    ADD CONSTRAINT "KiserletLepesVegreHajtasValasztottLehetoseg_pkey" PRIMARY KEY (kiserletlepesvegrehajtasid, valaszthatoid);


--
-- Name: kiserletlepesvegrehajtasvizsgalat KiserletLepesVegreHajtasVizsgalat_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvizsgalat
    ADD CONSTRAINT "KiserletLepesVegreHajtasVizsgalat_pkey" PRIMARY KEY (kiserletlepesvegrehajtasid, alanyvizsgalatid);


--
-- Name: kiserletlepesvegrehajtasesemeny KiserletLepesVegrehajtasEsemeny_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasesemeny
    ADD CONSTRAINT "KiserletLepesVegrehajtasEsemeny_pkey" PRIMARY KEY (hanyadik, kiserletlepesvegrehajtasid);


--
-- Name: kiserletlepesvegrehajtas KiserletLepesVegrehajtas_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtas
    ADD CONSTRAINT "KiserletLepesVegrehajtas_id_key" UNIQUE (id);


--
-- Name: kiserletlepesvegrehajtas KiserletLepesVegrehajtas_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtas
    ADD CONSTRAINT "KiserletLepesVegrehajtas_pkey" PRIMARY KEY (probalkozas, klcshozzarendelesvegrehajtasid, kiserletlepesid, alanyid);


--
-- Name: kiserletlepes KiserletLepes_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepes
    ADD CONSTRAINT "KiserletLepes_id_key" UNIQUE (id);


--
-- Name: kiserletlepes KiserletLepes_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepes
    ADD CONSTRAINT "KiserletLepes_pkey" PRIMARY KEY (kiserletlepescsoportid, elnevezes);


--
-- Name: kiserletresztvevo KiserletResztvevo_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletresztvevo
    ADD CONSTRAINT "KiserletResztvevo_pkey" PRIMARY KEY (klcshozzarendelesvegrehajtasid, alanyid);


--
-- Name: kiserletvegrehajtas KiserletVegrehajtas_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletvegrehajtas
    ADD CONSTRAINT "KiserletVegrehajtas_id_key" UNIQUE (id);


--
-- Name: kiserletvegrehajtas KiserletVegrehajtas_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletvegrehajtas
    ADD CONSTRAINT "KiserletVegrehajtas_pkey" PRIMARY KEY (kiserletid, mikor);


--
-- Name: kiserlet Kiserlet_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserlet
    ADD CONSTRAINT "Kiserlet_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: kiserlet Kiserlet_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserlet
    ADD CONSTRAINT "Kiserlet_pkey" PRIMARY KEY (id);


--
-- Name: kiserletlepesfeltetelesugras KiserletlepesFeltetelesUgras_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugras
    ADD CONSTRAINT "KiserletlepesFeltetelesUgras_id_key" UNIQUE (id);


--
-- Name: kiserletlepesfeltetelesugras KiserletlepesFeltetelesUgras_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugras
    ADD CONSTRAINT "KiserletlepesFeltetelesUgras_pkey" PRIMARY KEY (kiserletlepesid, kiserletlepesidcel);


--
-- Name: lepesheztartozovizsgalat LepeshezTartozoVizsgalat_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.lepesheztartozovizsgalat
    ADD CONSTRAINT "LepeshezTartozoVizsgalat_pkey" PRIMARY KEY (kiserletlepesid, vizsgalatelojegyzesid);


--
-- Name: megetetendoetelek MegetetendoEtelek_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.megetetendoetelek
    ADD CONSTRAINT "MegetetendoEtelek_pkey" PRIMARY KEY (kiserletlepesid, anyagid);


--
-- Name: megetetettetelek MegetetettEtelek_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.megetetettetelek
    ADD CONSTRAINT "MegetetettEtelek_pkey" PRIMARY KEY (etetesid, anyagid);


--
-- Name: munkatars Munkatars_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.munkatars
    ADD CONSTRAINT "Munkatars_pkey" PRIMARY KEY (id);


--
-- Name: oklevel Oklevel_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.oklevel
    ADD CONSTRAINT "Oklevel_pkey" PRIMARY KEY (munkatarsid, oktatasiintezmenyid, szakid, kezdeseve);


--
-- Name: oktatasiintezmeny OktatasiIntezmeny_nev_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.oktatasiintezmeny
    ADD CONSTRAINT "OktatasiIntezmeny_nev_key" UNIQUE (nev);


--
-- Name: oktatasiintezmeny OktatasiIntezmeny_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.oktatasiintezmeny
    ADD CONSTRAINT "OktatasiIntezmeny_pkey" PRIMARY KEY (id);


--
-- Name: receptvariacionev ReceptVariacioNev_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.receptvariacionev
    ADD CONSTRAINT "ReceptVariacioNev_pkey" PRIMARY KEY (id);


--
-- Name: recept Recept_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.recept
    ADD CONSTRAINT "Recept_pkey" PRIMARY KEY (anyagid, anyagidosszetevo, variacionevid);


--
-- Name: szak Szak_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szak
    ADD CONSTRAINT "Szak_pkey" PRIMARY KEY (id);


--
-- Name: szak Szak_szakmegnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szak
    ADD CONSTRAINT "Szak_szakmegnevezes_key" UNIQUE (szakmegnevezes);


--
-- Name: szemely Személy_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szemely
    ADD CONSTRAINT "Személy_pkey" PRIMARY KEY (id);


--
-- Name: szemely Személy_szemelyiSzam_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szemely
    ADD CONSTRAINT "Személy_szemelyiSzam_key" UNIQUE (szemelyiszam);


--
-- Name: szerepkor Szerepkor_elvevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szerepkor
    ADD CONSTRAINT "Szerepkor_elvevezes_key" UNIQUE (elvevezes);


--
-- Name: szerepkor Szerepkor_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szerepkor
    ADD CONSTRAINT "Szerepkor_pkey" PRIMARY KEY (id);


--
-- Name: tapasztalat Tapasztalat_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tapasztalat
    ADD CONSTRAINT "Tapasztalat_pkey" PRIMARY KEY (munkatarsid, cegid, munkaviszonykezdete);


--
-- Name: tartalmaz Tartalmaz_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tartalmaz
    ADD CONSTRAINT "Tartalmaz_pkey" PRIMARY KEY (vizsgalatid, alkotoreszid, anyagid);


--
-- Name: tulajdonsag Tualjdonsag_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tulajdonsag
    ADD CONSTRAINT "Tualjdonsag_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: tulajdonsag Tualjdonsag_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tulajdonsag
    ADD CONSTRAINT "Tualjdonsag_pkey" PRIMARY KEY (id);


--
-- Name: tunetallapot TunetAllapot_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tunetallapot
    ADD CONSTRAINT "TunetAllapot_pkey" PRIMARY KEY (vizsgalatid, tunetid);


--
-- Name: tunet Tunet_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tunet
    ADD CONSTRAINT "Tunet_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: tunet Tunet_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tunet
    ADD CONSTRAINT "Tunet_pkey" PRIMARY KEY (id);


--
-- Name: valasztasilehetoseg ValasztasiLehetoseg_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valasztasilehetoseg
    ADD CONSTRAINT "ValasztasiLehetoseg_pkey" PRIMARY KEY (kiserletlepesid, valaszthatoid);


--
-- Name: valaszthatoalanytipus ValaszthatoAlanyTipus_id_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalanytipus
    ADD CONSTRAINT "ValaszthatoAlanyTipus_id_key" UNIQUE (id);


--
-- Name: valaszthatoalanytipus ValaszthatoAlanyTipus_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalanytipus
    ADD CONSTRAINT "ValaszthatoAlanyTipus_pkey" PRIMARY KEY (kiserletlepesid, vizsgalatelojegyzesid);


--
-- Name: valaszthatoalany ValaszthatoAlany_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalany
    ADD CONSTRAINT "ValaszthatoAlany_pkey" PRIMARY KEY (klcshozzarendelesvegrehajtasid, alanyid);


--
-- Name: valaszthatoanyag ValaszthatoAnyag_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoanyag
    ADD CONSTRAINT "ValaszthatoAnyag_pkey" PRIMARY KEY (kiserletlepesid, anyagid);


--
-- Name: valaszthato Valaszthato_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthato
    ADD CONSTRAINT "Valaszthato_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: valaszthato Valaszthato_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthato
    ADD CONSTRAINT "Valaszthato_pkey" PRIMARY KEY (id);


--
-- Name: vizsgalatelojegyzes VizsgalatElojegyzes_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.vizsgalatelojegyzes
    ADD CONSTRAINT "VizsgalatElojegyzes_pkey" PRIMARY KEY (id);


--
-- Name: vizsgalattipus VizsgalatTipus_elnevezes_key; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.vizsgalattipus
    ADD CONSTRAINT "VizsgalatTipus_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: vizsgalattipus VizsgalatTipus_pkey; Type: CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.vizsgalattipus
    ADD CONSTRAINT "VizsgalatTipus_pkey" PRIMARY KEY (id);


--
-- Name: engedelyezettsema EngedelyezettSema_pkey; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.engedelyezettsema
    ADD CONSTRAINT "EngedelyezettSema_pkey" PRIMARY KEY (jogcsoportid, semaid);


--
-- Name: felhasznalo Felhasznalo_email_key; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.felhasznalo
    ADD CONSTRAINT "Felhasznalo_email_key" UNIQUE (email);


--
-- Name: felhasznalo Felhasznalo_felhasznalonev_key; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.felhasznalo
    ADD CONSTRAINT "Felhasznalo_felhasznalonev_key" UNIQUE (felhasznalonev);


--
-- Name: felhasznalo Felhasznalo_pkey; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.felhasznalo
    ADD CONSTRAINT "Felhasznalo_pkey" PRIMARY KEY (id);


--
-- Name: hozzafereskategoria HozzaferesKategoria_nev_key; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.hozzafereskategoria
    ADD CONSTRAINT "HozzaferesKategoria_nev_key" UNIQUE (nev);


--
-- Name: hozzafereskategoria HozzaferesKategoria_pkey; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.hozzafereskategoria
    ADD CONSTRAINT "HozzaferesKategoria_pkey" PRIMARY KEY (id);


--
-- Name: jogcsoporthozzarendeles JogCsoportHozzarendeles_pkey; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.jogcsoporthozzarendeles
    ADD CONSTRAINT "JogCsoportHozzarendeles_pkey" PRIMARY KEY (jogcsoportid, felhasznaloid);


--
-- Name: jogcsoport JogCsoport_pkey; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.jogcsoport
    ADD CONSTRAINT "JogCsoport_pkey" PRIMARY KEY (id);


--
-- Name: bearertoken bearertoken_token_pkey; Type: CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.bearertoken
    ADD CONSTRAINT bearertoken_token_pkey PRIMARY KEY (token);


--
-- Name: mertekegyseg Mertekegyseg_elnevezes_key; Type: CONSTRAINT; Schema: szotar; Owner: postgres
--

ALTER TABLE ONLY szotar.mertekegyseg
    ADD CONSTRAINT "Mertekegyseg_elnevezes_key" UNIQUE (elnevezes);


--
-- Name: mertekegyseg Mertekegyseg_pkey; Type: CONSTRAINT; Schema: szotar; Owner: postgres
--

ALTER TABLE ONLY szotar.mertekegyseg
    ADD CONSTRAINT "Mertekegyseg_pkey" PRIMARY KEY (id);


--
-- Name: mertekegyseg Mertekegyseg_rovidites_key; Type: CONSTRAINT; Schema: szotar; Owner: postgres
--

ALTER TABLE ONLY szotar.mertekegyseg
    ADD CONSTRAINT "Mertekegyseg_rovidites_key" UNIQUE (rovidites);


--
-- Name: U_KiserletLepesSorrend; Type: INDEX; Schema: admin; Owner: postgres
--

CREATE UNIQUE INDEX "U_KiserletLepesSorrend" ON admin.kiserletlepessorrend USING btree (kiserletlepesid, hanyadik, kiserletlepesidszuloelem, csoportszam);


--
-- Name: engedelyezettsema trg_validate_semaid; Type: TRIGGER; Schema: sysadmin; Owner: postgres
--

CREATE TRIGGER trg_validate_semaid BEFORE INSERT OR UPDATE ON sysadmin.engedelyezettsema FOR EACH ROW EXECUTE FUNCTION sysadmin.validate_semaid();


--
-- Name: alanytulajdonsag AlanyTulajdonsag_tulajdonsagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanytulajdonsag
    ADD CONSTRAINT "AlanyTulajdonsag_tulajdonsagID_fkey" FOREIGN KEY (tulajdonsagid) REFERENCES admin.tulajdonsag(id);


--
-- Name: alanytulajdonsag AlanyTulajdonsag_vizsgalatID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanytulajdonsag
    ADD CONSTRAINT "AlanyTulajdonsag_vizsgalatID_fkey" FOREIGN KEY (vizsgalatid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: alanyvizsgalat AlanyVizsgalat_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanyvizsgalat
    ADD CONSTRAINT "AlanyVizsgalat_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: alanyvizsgalat AlanyVizsgalat_vizsgalatElojegyzesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alanyvizsgalat
    ADD CONSTRAINT "AlanyVizsgalat_vizsgalatElojegyzesID_fkey" FOREIGN KEY (vizsgalatelojegyzesid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: alany Alany_fajID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alany
    ADD CONSTRAINT "Alany_fajID_fkey" FOREIGN KEY (fajid) REFERENCES admin.faj(id);


--
-- Name: alany Alany_id_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alany
    ADD CONSTRAINT "Alany_id_fkey" FOREIGN KEY (id) REFERENCES sysadmin.felhasznalo(id);


--
-- Name: alkotoresztulajdonsag AlkotoreszTulajdonsag_alkotoreszID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresztulajdonsag
    ADD CONSTRAINT "AlkotoreszTulajdonsag_alkotoreszID_fkey" FOREIGN KEY (alkotoreszid) REFERENCES admin.alkotoresz(id);


--
-- Name: alkotoresztulajdonsag AlkotoreszTulajdonsag_tulajdonsagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresztulajdonsag
    ADD CONSTRAINT "AlkotoreszTulajdonsag_tulajdonsagID_fkey" FOREIGN KEY (tulajdonsagid) REFERENCES admin.tulajdonsag(id);


--
-- Name: alkotoresztulajdonsag AlkotoreszTulajdonsag_vizsgalatID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresztulajdonsag
    ADD CONSTRAINT "AlkotoreszTulajdonsag_vizsgalatID_fkey" FOREIGN KEY (vizsgalatid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: alkotoresz Alkotoresz_alkotoreszIDazonbelul_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresz
    ADD CONSTRAINT "Alkotoresz_alkotoreszIDazonbelul_fkey" FOREIGN KEY (alkotoreszidazonbelul) REFERENCES admin.alkotoresz(id);


--
-- Name: alkotoresz Alkotoresz_alkotoreszTipusID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.alkotoresz
    ADD CONSTRAINT "Alkotoresz_alkotoreszTipusID_fkey" FOREIGN KEY (alkotoresztipusid) REFERENCES admin.alkotoresztipus(id);


--
-- Name: anyag Anyag_mertekegysegID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.anyag
    ADD CONSTRAINT "Anyag_mertekegysegID_fkey" FOREIGN KEY (mertekegysegid) REFERENCES szotar.mertekegyseg(id);


--
-- Name: atjaras Atjaras_epuletHelysegID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.atjaras
    ADD CONSTRAINT "Atjaras_epuletHelysegID_fkey" FOREIGN KEY (epulethelysegid) REFERENCES admin.epulethelyseg(id);


--
-- Name: atjaras Atjaras_epuletHelysegIDcel_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.atjaras
    ADD CONSTRAINT "Atjaras_epuletHelysegIDcel_fkey" FOREIGN KEY (epulethelysegidcel) REFERENCES admin.epulethelyseg(id);


--
-- Name: epulethelysegferohely EpuletHelysegFerohely_epuletHelysegID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.epulethelysegferohely
    ADD CONSTRAINT "EpuletHelysegFerohely_epuletHelysegID_fkey" FOREIGN KEY (epulethelysegid) REFERENCES admin.epulethelyseg(id);


--
-- Name: epulethelysegferohely EpuletHelysegFerohely_kiserletCsoportID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.epulethelysegferohely
    ADD CONSTRAINT "EpuletHelysegFerohely_kiserletCsoportID_fkey" FOREIGN KEY (kiserletcsoportid) REFERENCES admin.kiserletlepescsoport(id);


--
-- Name: erintettalkotoresz ErintettAlkotoresz_alkotoreszID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.erintettalkotoresz
    ADD CONSTRAINT "ErintettAlkotoresz_alkotoreszID_fkey" FOREIGN KEY (alkotoreszid) REFERENCES admin.alkotoresz(id);


--
-- Name: erintettalkotoresz ErintettAlkotoresz_tunetID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.erintettalkotoresz
    ADD CONSTRAINT "ErintettAlkotoresz_tunetID_fkey" FOREIGN KEY (tunetid) REFERENCES admin.tunet(id);


--
-- Name: etetes Etetes_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.etetes
    ADD CONSTRAINT "Etetes_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: etetes Etetes_munkatarsIDseged_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.etetes
    ADD CONSTRAINT "Etetes_munkatarsIDseged_fkey" FOREIGN KEY (munkatarsidseged) REFERENCES admin.munkatars(id);


--
-- Name: fajalkotoresze FajAlkotoresze_alkotoreszID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.fajalkotoresze
    ADD CONSTRAINT "FajAlkotoresze_alkotoreszID_fkey" FOREIGN KEY (alkotoreszid) REFERENCES admin.alkotoresz(id);


--
-- Name: fajalkotoresze FajAlkotoresze_fajID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.fajalkotoresze
    ADD CONSTRAINT "FajAlkotoresze_fajID_fkey" FOREIGN KEY (fajid) REFERENCES admin.faj(id);


--
-- Name: halottalany HalottAlany_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.halottalany
    ADD CONSTRAINT "HalottAlany_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: klcshozzarendelesvegrehajtas KLCsHozzarendelesVegrehajtas_kLCsHozzarendelesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendelesvegrehajtas
    ADD CONSTRAINT "KLCsHozzarendelesVegrehajtas_kLCsHozzarendelesID_fkey" FOREIGN KEY (klcshozzarendelesid) REFERENCES admin.klcshozzarendeles(id);


--
-- Name: klcshozzarendelesvegrehajtas KLCsHozzarendelesVegrehajtas_kiserletVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendelesvegrehajtas
    ADD CONSTRAINT "KLCsHozzarendelesVegrehajtas_kiserletVegrehajtasID_fkey" FOREIGN KEY (kiserletvegrehajtasid) REFERENCES admin.kiserletvegrehajtas(id);


--
-- Name: klcshozzarendeles KLCsHozzarendeles_kiserletID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendeles
    ADD CONSTRAINT "KLCsHozzarendeles_kiserletID_fkey" FOREIGN KEY (kiserletid) REFERENCES admin.kiserlet(id);


--
-- Name: klcshozzarendeles KLCsHozzarendeles_kiserletLepesCsoportID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.klcshozzarendeles
    ADD CONSTRAINT "KLCsHozzarendeles_kiserletLepesCsoportID_fkey" FOREIGN KEY (kiserletlepescsoportid) REFERENCES admin.kiserletlepescsoport(id);


--
-- Name: kiserletlepesfeltetelesugrasfeltetel KiserletLepesFeltetelesUgrasF_kiserletlepesFeltetelesUgras_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugrasfeltetel
    ADD CONSTRAINT "KiserletLepesFeltetelesUgrasF_kiserletlepesFeltetelesUgras_fkey" FOREIGN KEY (kiserletlepesfeltetelesugrasid) REFERENCES admin.kiserletlepesfeltetelesugras(id);


--
-- Name: kiserletlepesfeltetelesugrasfeltetel KiserletLepesFeltetelesUgrasFeltetel_vizsgalatElojegyzesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugrasfeltetel
    ADD CONSTRAINT "KiserletLepesFeltetelesUgrasFeltetel_vizsgalatElojegyzesID_fkey" FOREIGN KEY (vizsgalatelojegyzesid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: kiserletlepessorrend KiserletLepesSorrend_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepessorrend
    ADD CONSTRAINT "KiserletLepesSorrend_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: kiserletlepessorrend KiserletLepesSorrend_kiserletLepesIDszuloelem_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepessorrend
    ADD CONSTRAINT "KiserletLepesSorrend_kiserletLepesIDszuloelem_fkey" FOREIGN KEY (kiserletlepesidszuloelem) REFERENCES admin.kiserletlepes(id);


--
-- Name: kiserletlepesvegrehajtasetetes KiserletLepesVegreHajtasEtetes_etetesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasetetes
    ADD CONSTRAINT "KiserletLepesVegreHajtasEtetes_etetesID_fkey" FOREIGN KEY (etetesid) REFERENCES admin.etetes(id);


--
-- Name: kiserletlepesvegrehajtasetetes KiserletLepesVegreHajtasEtetes_kiserletLepesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasetetes
    ADD CONSTRAINT "KiserletLepesVegreHajtasEtetes_kiserletLepesVegrehajtasID_fkey" FOREIGN KEY (kiserletlepesvegrehajtasid) REFERENCES admin.kiserletlepesvegrehajtas(id);


--
-- Name: kiserletlepesvegrehajtasvalasztottlehetoseg KiserletLepesVegreHajtasValasz_kiserletLepesVegrehajtasID_fkey1; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvalasztottlehetoseg
    ADD CONSTRAINT "KiserletLepesVegreHajtasValasz_kiserletLepesVegrehajtasID_fkey1" FOREIGN KEY (kiserletlepesvegrehajtasid) REFERENCES admin.kiserletlepesvegrehajtas(id);


--
-- Name: kiserletlepesvegrehajtasvalasztottalany KiserletLepesVegreHajtasValaszt_kiserletLepesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvalasztottalany
    ADD CONSTRAINT "KiserletLepesVegreHajtasValaszt_kiserletLepesVegrehajtasID_fkey" FOREIGN KEY (kiserletlepesvegrehajtasid) REFERENCES admin.kiserletlepesvegrehajtas(id);


--
-- Name: kiserletlepesvegrehajtasvalasztottalany KiserletLepesVegreHajtasValasztottAlany_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvalasztottalany
    ADD CONSTRAINT "KiserletLepesVegreHajtasValasztottAlany_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: kiserletlepesvegrehajtasvalasztottlehetoseg KiserletLepesVegreHajtasValasztottLehetoseg_valaszthatoID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvalasztottlehetoseg
    ADD CONSTRAINT "KiserletLepesVegreHajtasValasztottLehetoseg_valaszthatoID_fkey" FOREIGN KEY (valaszthatoid) REFERENCES admin.valaszthato(id);


--
-- Name: kiserletlepesvegrehajtasvizsgalat KiserletLepesVegreHajtasVizsgal_kiserletLepesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvizsgalat
    ADD CONSTRAINT "KiserletLepesVegreHajtasVizsgal_kiserletLepesVegrehajtasID_fkey" FOREIGN KEY (kiserletlepesvegrehajtasid) REFERENCES admin.kiserletlepesvegrehajtas(id);


--
-- Name: kiserletlepesvegrehajtasvizsgalat KiserletLepesVegreHajtasVizsgalat_alanyVizsgalatID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasvizsgalat
    ADD CONSTRAINT "KiserletLepesVegreHajtasVizsgalat_alanyVizsgalatID_fkey" FOREIGN KEY (alanyvizsgalatid) REFERENCES admin.alanyvizsgalat(id);


--
-- Name: kiserletlepesvegrehajtasesemeny KiserletLepesVegrehajtasEsemeny_kiserletLepesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtasesemeny
    ADD CONSTRAINT "KiserletLepesVegrehajtasEsemeny_kiserletLepesVegrehajtasID_fkey" FOREIGN KEY (kiserletlepesvegrehajtasid) REFERENCES admin.kiserletlepesvegrehajtas(id);


--
-- Name: kiserletlepesvegrehajtas KiserletLepesVegrehajtas_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtas
    ADD CONSTRAINT "KiserletLepesVegrehajtas_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: kiserletlepesvegrehajtas KiserletLepesVegrehajtas_kLCsHozzarendelesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtas
    ADD CONSTRAINT "KiserletLepesVegrehajtas_kLCsHozzarendelesVegrehajtasID_fkey" FOREIGN KEY (klcshozzarendelesvegrehajtasid) REFERENCES admin.klcshozzarendelesvegrehajtas(id);


--
-- Name: kiserletlepesvegrehajtas KiserletLepesVegrehajtas_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtas
    ADD CONSTRAINT "KiserletLepesVegrehajtas_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: kiserletlepesvegrehajtas KiserletLepesVegrehajtas_munkatarsID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesvegrehajtas
    ADD CONSTRAINT "KiserletLepesVegrehajtas_munkatarsID_fkey" FOREIGN KEY (munkatarsid) REFERENCES admin.munkatars(id);


--
-- Name: kiserletlepes KiserletLepes_epuletHelysegID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepes
    ADD CONSTRAINT "KiserletLepes_epuletHelysegID_fkey" FOREIGN KEY (epulethelysegid) REFERENCES admin.epulethelyseg(id);


--
-- Name: kiserletlepes KiserletLepes_kiserletLepesCsoportID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepes
    ADD CONSTRAINT "KiserletLepes_kiserletLepesCsoportID_fkey" FOREIGN KEY (kiserletlepescsoportid) REFERENCES admin.kiserletlepescsoport(id);


--
-- Name: kiserletresztvevo KiserletResztvevo_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletresztvevo
    ADD CONSTRAINT "KiserletResztvevo_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: kiserletresztvevo KiserletResztvevo_kLCsHozzarendelesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletresztvevo
    ADD CONSTRAINT "KiserletResztvevo_kLCsHozzarendelesVegrehajtasID_fkey" FOREIGN KEY (klcshozzarendelesvegrehajtasid) REFERENCES admin.klcshozzarendelesvegrehajtas(id);


--
-- Name: kiserletvegrehajtas KiserletVegrehajtas_kiserletID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletvegrehajtas
    ADD CONSTRAINT "KiserletVegrehajtas_kiserletID_fkey" FOREIGN KEY (kiserletid) REFERENCES admin.kiserlet(id);


--
-- Name: kiserletvegrehajtas KiserletVegrehajtas_munkatarsID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletvegrehajtas
    ADD CONSTRAINT "KiserletVegrehajtas_munkatarsID_fkey" FOREIGN KEY (munkatarsid) REFERENCES admin.munkatars(id);


--
-- Name: kiserletlepesfeltetelesugras KiserletlepesFeltetelesUgras_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugras
    ADD CONSTRAINT "KiserletlepesFeltetelesUgras_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: kiserletlepesfeltetelesugras KiserletlepesFeltetelesUgras_kiserletLepesIDcel_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.kiserletlepesfeltetelesugras
    ADD CONSTRAINT "KiserletlepesFeltetelesUgras_kiserletLepesIDcel_fkey" FOREIGN KEY (kiserletlepesidcel) REFERENCES admin.kiserletlepes(id);


--
-- Name: lepesheztartozovizsgalat LepeshezTartozoVizsgalat_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.lepesheztartozovizsgalat
    ADD CONSTRAINT "LepeshezTartozoVizsgalat_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: lepesheztartozovizsgalat LepeshezTartozoVizsgalat_vizsgalatElojegyzesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.lepesheztartozovizsgalat
    ADD CONSTRAINT "LepeshezTartozoVizsgalat_vizsgalatElojegyzesID_fkey" FOREIGN KEY (vizsgalatelojegyzesid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: megetetendoetelek MegetetendoEtelek_anyagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.megetetendoetelek
    ADD CONSTRAINT "MegetetendoEtelek_anyagID_fkey" FOREIGN KEY (anyagid) REFERENCES admin.anyag(id);


--
-- Name: megetetendoetelek MegetetendoEtelek_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.megetetendoetelek
    ADD CONSTRAINT "MegetetendoEtelek_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: megetetettetelek MegetetettEtelek_anyagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.megetetettetelek
    ADD CONSTRAINT "MegetetettEtelek_anyagID_fkey" FOREIGN KEY (anyagid) REFERENCES admin.anyag(id);


--
-- Name: megetetettetelek MegetetettEtelek_etetesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.megetetettetelek
    ADD CONSTRAINT "MegetetettEtelek_etetesID_fkey" FOREIGN KEY (etetesid) REFERENCES admin.etetes(id);


--
-- Name: munkatars Munkatars_id_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.munkatars
    ADD CONSTRAINT "Munkatars_id_fkey" FOREIGN KEY (id) REFERENCES sysadmin.felhasznalo(id);


--
-- Name: munkatars Munkatars_szerepkorID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.munkatars
    ADD CONSTRAINT "Munkatars_szerepkorID_fkey" FOREIGN KEY (szerepkorid) REFERENCES admin.szerepkor(id);


--
-- Name: oklevel Oklevel_munkatarsID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.oklevel
    ADD CONSTRAINT "Oklevel_munkatarsID_fkey" FOREIGN KEY (munkatarsid) REFERENCES admin.munkatars(id);


--
-- Name: oklevel Oklevel_oktatasiIntezmenyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.oklevel
    ADD CONSTRAINT "Oklevel_oktatasiIntezmenyID_fkey" FOREIGN KEY (oktatasiintezmenyid) REFERENCES admin.oktatasiintezmeny(id);


--
-- Name: oklevel Oklevel_szakID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.oklevel
    ADD CONSTRAINT "Oklevel_szakID_fkey" FOREIGN KEY (szakid) REFERENCES admin.szak(id);


--
-- Name: recept Recept_anyagIDOsszetevo_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.recept
    ADD CONSTRAINT "Recept_anyagIDOsszetevo_fkey" FOREIGN KEY (anyagidosszetevo) REFERENCES admin.anyag(id);


--
-- Name: recept Recept_anyagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.recept
    ADD CONSTRAINT "Recept_anyagID_fkey" FOREIGN KEY (anyagid) REFERENCES admin.anyag(id);


--
-- Name: recept Recept_variacioNevID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.recept
    ADD CONSTRAINT "Recept_variacioNevID_fkey" FOREIGN KEY (variacionevid) REFERENCES admin.receptvariacionev(id);


--
-- Name: szak Szak_oktatasiIntezmenyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.szak
    ADD CONSTRAINT "Szak_oktatasiIntezmenyID_fkey" FOREIGN KEY (oktatasiintezmenyid) REFERENCES admin.oktatasiintezmeny(id);


--
-- Name: tapasztalat Tapasztalat_cegID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tapasztalat
    ADD CONSTRAINT "Tapasztalat_cegID_fkey" FOREIGN KEY (cegid) REFERENCES admin.ceg(id);


--
-- Name: tapasztalat Tapasztalat_munkatarsID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tapasztalat
    ADD CONSTRAINT "Tapasztalat_munkatarsID_fkey" FOREIGN KEY (munkatarsid) REFERENCES admin.munkatars(id);


--
-- Name: tartalmaz Tartalmaz_alkotoreszID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tartalmaz
    ADD CONSTRAINT "Tartalmaz_alkotoreszID_fkey" FOREIGN KEY (alkotoreszid) REFERENCES admin.alkotoresz(id);


--
-- Name: tartalmaz Tartalmaz_anyagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tartalmaz
    ADD CONSTRAINT "Tartalmaz_anyagID_fkey" FOREIGN KEY (anyagid) REFERENCES admin.anyag(id);


--
-- Name: tartalmaz Tartalmaz_vizsgalatID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tartalmaz
    ADD CONSTRAINT "Tartalmaz_vizsgalatID_fkey" FOREIGN KEY (vizsgalatid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: tulajdonsag Tualjdonsag_mertekegysegID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tulajdonsag
    ADD CONSTRAINT "Tualjdonsag_mertekegysegID_fkey" FOREIGN KEY (mertekegysegid) REFERENCES szotar.mertekegyseg(id);


--
-- Name: tunetallapot TunetAllapot_tunetID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tunetallapot
    ADD CONSTRAINT "TunetAllapot_tunetID_fkey" FOREIGN KEY (tunetid) REFERENCES admin.tunet(id);


--
-- Name: tunetallapot TunetAllapot_vizsgalatID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.tunetallapot
    ADD CONSTRAINT "TunetAllapot_vizsgalatID_fkey" FOREIGN KEY (vizsgalatid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: valasztasilehetoseg ValasztasiLehetoseg_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valasztasilehetoseg
    ADD CONSTRAINT "ValasztasiLehetoseg_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: valasztasilehetoseg ValasztasiLehetoseg_valaszthatoID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valasztasilehetoseg
    ADD CONSTRAINT "ValasztasiLehetoseg_valaszthatoID_fkey" FOREIGN KEY (valaszthatoid) REFERENCES admin.valaszthato(id);


--
-- Name: valaszthatoalanytipus ValaszthatoAlanyTipus_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalanytipus
    ADD CONSTRAINT "ValaszthatoAlanyTipus_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: valaszthatoalanytipus ValaszthatoAlanyTipus_vizsgalatElojegyzesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalanytipus
    ADD CONSTRAINT "ValaszthatoAlanyTipus_vizsgalatElojegyzesID_fkey" FOREIGN KEY (vizsgalatelojegyzesid) REFERENCES admin.vizsgalatelojegyzes(id);


--
-- Name: valaszthatoalany ValaszthatoAlany_alanyID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalany
    ADD CONSTRAINT "ValaszthatoAlany_alanyID_fkey" FOREIGN KEY (alanyid) REFERENCES admin.alany(id);


--
-- Name: valaszthatoalany ValaszthatoAlany_kLCsHozzarendelesVegrehajtasID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalany
    ADD CONSTRAINT "ValaszthatoAlany_kLCsHozzarendelesVegrehajtasID_fkey" FOREIGN KEY (klcshozzarendelesvegrehajtasid) REFERENCES admin.klcshozzarendelesvegrehajtas(id);


--
-- Name: valaszthatoalany ValaszthatoAlany_valaszthatoAlanyTipusID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoalany
    ADD CONSTRAINT "ValaszthatoAlany_valaszthatoAlanyTipusID_fkey" FOREIGN KEY (valaszthatoalanytipusid) REFERENCES admin.valaszthatoalanytipus(id);


--
-- Name: valaszthatoanyag ValaszthatoAnyag_anyagID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoanyag
    ADD CONSTRAINT "ValaszthatoAnyag_anyagID_fkey" FOREIGN KEY (anyagid) REFERENCES admin.anyag(id);


--
-- Name: valaszthatoanyag ValaszthatoAnyag_kiserletLepesID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.valaszthatoanyag
    ADD CONSTRAINT "ValaszthatoAnyag_kiserletLepesID_fkey" FOREIGN KEY (kiserletlepesid) REFERENCES admin.kiserletlepes(id);


--
-- Name: vizsgalatelojegyzes VizsgalatElojegyzes_vizsgalatTipusID_fkey; Type: FK CONSTRAINT; Schema: admin; Owner: postgres
--

ALTER TABLE ONLY admin.vizsgalatelojegyzes
    ADD CONSTRAINT "VizsgalatElojegyzes_vizsgalatTipusID_fkey" FOREIGN KEY (vizsgalattipusid) REFERENCES admin.vizsgalattipus(id);


--
-- Name: engedelyezettsema EngedelyezettSema_hozzaferesKategoriaID_fkey; Type: FK CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.engedelyezettsema
    ADD CONSTRAINT "EngedelyezettSema_hozzaferesKategoriaID_fkey" FOREIGN KEY (hozzafereskategoriaid) REFERENCES sysadmin.hozzafereskategoria(id);


--
-- Name: engedelyezettsema EngedelyezettSema_jogCsoportID_fkey; Type: FK CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.engedelyezettsema
    ADD CONSTRAINT "EngedelyezettSema_jogCsoportID_fkey" FOREIGN KEY (jogcsoportid) REFERENCES sysadmin.jogcsoport(id);


--
-- Name: jogcsoporthozzarendeles JogCsoportHozzarendeles_felhasznaloID_fkey; Type: FK CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.jogcsoporthozzarendeles
    ADD CONSTRAINT "JogCsoportHozzarendeles_felhasznaloID_fkey" FOREIGN KEY (felhasznaloid) REFERENCES sysadmin.felhasznalo(id);


--
-- Name: jogcsoporthozzarendeles JogCsoportHozzarendeles_jogCsoportID_fkey; Type: FK CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.jogcsoporthozzarendeles
    ADD CONSTRAINT "JogCsoportHozzarendeles_jogCsoportID_fkey" FOREIGN KEY (jogcsoportid) REFERENCES sysadmin.jogcsoport(id);


--
-- Name: bearertoken bearertoken_felhasznaloid_fk; Type: FK CONSTRAINT; Schema: sysadmin; Owner: postgres
--

ALTER TABLE ONLY sysadmin.bearertoken
    ADD CONSTRAINT bearertoken_felhasznaloid_fk FOREIGN KEY (felhasznaloid) REFERENCES sysadmin.felhasznalo(id);


--
-- Name: TABLE alanyvizsgalat; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.alanyvizsgalat TO user_2;


--
-- Name: TABLE alany; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.alany TO user_2;


--
-- Name: TABLE alkotoresztipus; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.alkotoresztipus TO user_2;


--
-- Name: TABLE alkotoresz; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.alkotoresz TO user_2;


--
-- Name: TABLE anyag; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.anyag TO user_2;


--
-- Name: TABLE ceg; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.ceg TO user_2;


--
-- Name: TABLE epulethelyseg; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.epulethelyseg TO user_2;


--
-- Name: TABLE etetes; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.etetes TO user_2;


--
-- Name: TABLE faj; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.faj TO user_2;


--
-- Name: TABLE klcshozzarendelesvegrehajtas; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.klcshozzarendelesvegrehajtas TO user_2;


--
-- Name: TABLE klcshozzarendeles; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.klcshozzarendeles TO user_2;


--
-- Name: TABLE kiserletlepescsoport; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepescsoport TO user_2;


--
-- Name: TABLE kiserletlepesvegrehajtasesemeny; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesvegrehajtasesemeny TO user_2;


--
-- Name: TABLE kiserletlepesvegrehajtas; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesvegrehajtas TO user_2;


--
-- Name: TABLE kiserletlepes; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepes TO user_2;


--
-- Name: TABLE kiserletvegrehajtas; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletvegrehajtas TO user_2;


--
-- Name: TABLE kiserlet; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserlet TO user_2;


--
-- Name: TABLE kiserletlepesfeltetelesugras; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesfeltetelesugras TO user_2;


--
-- Name: TABLE munkatars; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.munkatars TO user_2;


--
-- Name: TABLE oktatasiintezmeny; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.oktatasiintezmeny TO user_2;


--
-- Name: TABLE szak; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.szak TO user_2;


--
-- Name: TABLE szemely; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.szemely TO user_2;


--
-- Name: TABLE szerepkor; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.szerepkor TO user_2;


--
-- Name: TABLE tulajdonsag; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.tulajdonsag TO user_2;


--
-- Name: TABLE tunet; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.tunet TO user_2;


--
-- Name: TABLE valaszthatoalanytipus; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.valaszthatoalanytipus TO user_2;


--
-- Name: TABLE valaszthato; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.valaszthato TO user_2;


--
-- Name: TABLE vizsgalatelojegyzes; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.vizsgalatelojegyzes TO user_2;


--
-- Name: TABLE vizsgalattipus; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.vizsgalattipus TO user_2;


--
-- Name: TABLE alanytulajdonsag; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.alanytulajdonsag TO user_2;


--
-- Name: TABLE alkotoresztulajdonsag; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.alkotoresztulajdonsag TO user_2;


--
-- Name: TABLE atjaras; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.atjaras TO user_2;


--
-- Name: TABLE epulethelysegferohely; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.epulethelysegferohely TO user_2;


--
-- Name: TABLE erintettalkotoresz; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.erintettalkotoresz TO user_2;


--
-- Name: TABLE etelintolarencia; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.etelintolarencia TO user_2;


--
-- Name: TABLE fajalkotoresze; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.fajalkotoresze TO user_2;


--
-- Name: TABLE halottalany; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.halottalany TO user_2;


--
-- Name: TABLE kiserletlepesfeltetelesugrasfeltetel; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesfeltetelesugrasfeltetel TO user_2;


--
-- Name: TABLE kiserletlepessorrend; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepessorrend TO user_2;


--
-- Name: TABLE kiserletlepesvegrehajtasetetes; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesvegrehajtasetetes TO user_2;


--
-- Name: TABLE kiserletlepesvegrehajtasvalasztottalany; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesvegrehajtasvalasztottalany TO user_2;


--
-- Name: TABLE kiserletlepesvegrehajtasvalasztottlehetoseg; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesvegrehajtasvalasztottlehetoseg TO user_2;


--
-- Name: TABLE kiserletlepesvegrehajtasvizsgalat; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletlepesvegrehajtasvizsgalat TO user_2;


--
-- Name: TABLE kiserletresztvevo; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.kiserletresztvevo TO user_2;


--
-- Name: TABLE lepesheztartozovizsgalat; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.lepesheztartozovizsgalat TO user_2;


--
-- Name: TABLE megetetendoetelek; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.megetetendoetelek TO user_2;


--
-- Name: TABLE megetetettetelek; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.megetetettetelek TO user_2;


--
-- Name: TABLE oklevel; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.oklevel TO user_2;


--
-- Name: TABLE recept; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.recept TO user_2;


--
-- Name: TABLE receptvariacionev; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.receptvariacionev TO user_2;


--
-- Name: TABLE tapasztalat; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.tapasztalat TO user_2;


--
-- Name: TABLE tartalmaz; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.tartalmaz TO user_2;


--
-- Name: TABLE tunetallapot; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.tunetallapot TO user_2;


--
-- Name: TABLE valasztasilehetoseg; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.valasztasilehetoseg TO user_2;


--
-- Name: TABLE valaszthatoalany; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.valaszthatoalany TO user_2;


--
-- Name: TABLE valaszthatoanyag; Type: ACL; Schema: admin; Owner: postgres
--

GRANT SELECT,INSERT,UPDATE ON TABLE admin.valaszthatoanyag TO user_2;


--
-- Name: TABLE personview; Type: ACL; Schema: users; Owner: postgres
--

GRANT SELECT,DELETE,UPDATE ON TABLE users.personview TO user_2;


--
-- Name: TABLE profileview; Type: ACL; Schema: users; Owner: postgres
--

GRANT SELECT,DELETE,UPDATE ON TABLE users.profileview TO user_2;


--
-- Name: DEFAULT PRIVILEGES FOR TABLES; Type: DEFAULT ACL; Schema: rszotar; Owner: postgres
--

ALTER DEFAULT PRIVILEGES FOR ROLE postgres IN SCHEMA rszotar GRANT SELECT ON TABLES  TO user_2;


--
-- Name: DEFAULT PRIVILEGES FOR TABLES; Type: DEFAULT ACL; Schema: szotar; Owner: postgres
--

ALTER DEFAULT PRIVILEGES FOR ROLE postgres IN SCHEMA szotar GRANT SELECT,DELETE,UPDATE ON TABLES  TO user_2;


--
-- Name: DEFAULT PRIVILEGES FOR SEQUENCES; Type: DEFAULT ACL; Schema: users; Owner: postgres
--

ALTER DEFAULT PRIVILEGES FOR ROLE postgres IN SCHEMA users GRANT SELECT,USAGE ON SEQUENCES  TO user_2;


--
-- Name: DEFAULT PRIVILEGES FOR TABLES; Type: DEFAULT ACL; Schema: users; Owner: postgres
--

ALTER DEFAULT PRIVILEGES FOR ROLE postgres IN SCHEMA users GRANT SELECT,DELETE,UPDATE ON TABLES  TO user_2;


--
-- PostgreSQL database dump complete
--

